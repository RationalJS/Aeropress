open AeroRoutes.Middleware;
module Option = Belt.Option;
module Dict = Js.Dict;
module Watch = NodeWatch;

exception AeroLessError(string);

/* TEMP */
type nodeCallback('e,'a) = (Js.Nullable.t('e), 'a) => unit;
[@bs.module "fs"]
external readFile : (string, nodeCallback(Js.Exn.t, BsNode.Node.Buffer.t)) => unit = "";
/* /TEMP */

type lessResult = {.
  "css": string
};
type lessError = {.
  "message": string,
  "file": string,
  "line": int,
  "column": int,
};
[@bs.module]
external less : { .
  "render": [@bs.meth] (string, Js.t({.}), nodeCallback(lessError, lessResult)) => unit
} = "less";

let isLessFile = (path) => Js.Re.test(path, [%re "/\\.less$/"]);

type cache =
  | Cold
  | Loading(Future.t( Js.Result.t(string,string) ))
  | Warm(string);


let file = (~debug=?, ~watchDir=?, ~includePaths=?, filePath) => {
  if ( ! Node.Fs.existsSync(filePath) ) {
    raise(AeroLessError("[AeroLess] File does not exist: " ++ filePath))
  };

  let debug' = Option.getWithDefault(debug, AeroConfig.dev);
  let watchDir' = Option.getWithDefault(watchDir, Node.Path.dirname(filePath));
  let includePaths' = Option.getWithDefault(includePaths, [filePath])
    |> List.append(_, ["node_modules"])
    |> Array.of_list;

  let cache = ref(Cold);

  let lessOptions = {
    "filename": filePath,
    "paths": includePaths',
    "outputStyle": "nested",
    "compress": AeroConfig.prod,
  };

  if ( AeroConfig.dev ) {
    Watch.config(~recursive=true, ~encoding="utf8", ())
    |> Watch.watch(watchDir', ~config=_, ())
    |> Watch.on(_, `change((. _, filenameBuffer) => {
      let file = filenameBuffer |> BsNode.Node.Buffer.toStringWithEncoding(_, "utf8");
      if ( isLessFile(file) ) {
        if (debug') { Js.log2("[AeroLess] Clearing cache for:", Node.Path.basename(file)) };
        cache := Cold;
      }
    }))
    |> ignore
  };

  /* Return the middleware */
  (r) => {
    let send = (result) => switch (result) {
      | Js.Result.Ok(css) =>
        r
        |> setHeader("content-type", "text/css")
        |> status(200)
        |> sendText(css);
      | Error(message) =>
        r |> status(500) |> sendText(message)
    };

    switch (cache^) {
      | Warm(css) => send(Js.Result.Ok(css))
      | Loading(futureCss) => futureCss |> Future.map(send) |> async
      | Cold =>
        let futureCss = Future.make(resolve =>
          readFile(filePath, (err, buffer) => switch (err |> Js.Nullable.toOption) {
            | Some(error) =>
              resolve @@ Js.Result.Error("Error reading LESS entry file: "
                      ++ Option.getWithDefault(Js.Exn.message(error), "<unknown error>"))
            | None =>
              let css = buffer |> BsNode.Node.Buffer.toStringWithEncoding(_, "utf8");
              /* For some reason re-using an options object causes errors in Less */
              let opts = Js.Obj.assign(Js.Obj.empty(), lessOptions);
              less##render(css, opts, (err, result) => switch (err |> Js.Nullable.toOption) {
                | Some(error) =>
                  Js.log("\n----------------------------\n");
                  Js.log2("LESS ERROR: ", error##message);
                  Js.log2("\tin file", error##file);
                  Js.log4("\ton line", error##line, "col", error##column);
                  Js.log("\n----------------------------\n");

                  resolve @@ Js.Result.Error(error##message)
                | None =>
                  resolve @@ Js.Result.Ok(result##css)
              })
          })
        );
        cache := Loading(futureCss);
        futureCss |> Future.map(send) |> async
    }
  }
};
