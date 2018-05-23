open AeroRoutes.Middleware;
module Option = Belt.Option;
module Dict = Js.Dict;
module Watch = NodeWatch;
module Buffer = BsNode.Node.Buffer;
module Stream = NodeExtHttp.NodeExtStream;

exception AeroCssError(string);

[@bs.module "pour-css"]
external bundle : string => Js.t(#Stream.readableStream) = "";

let bufferStreamtoFuture = (stream) => Future.make(resolve => {
  let data = ref(Buffer.fromString(""));

  stream
  |> Stream.ReadableStream.onData(Stream.Buffer, buf => {
    data := Buffer.concat([|data^, buf|])
  })
  |> Stream.ReadableStream.onFinish(e => switch(e |> Js.Nullable.toOption) {
    | Some(error) => resolve @@ Js.Result.Error(
        error |. Js.Exn.message |. Option.getWithDefault("unknown error")
      )
    | None => resolve @@ Js.Result.Ok(data^)
  })
});

let isCssFile = (path) => Js.Re.test(path, [%re "/\\.(css)$/"]);

type cache =
  | Cold
  | Loading(Future.t( Js.Result.t(string,string) ))
  | Warm(string);

let mapFutureResult = (f, future) => future |. Future.map(r => switch(r) {
  | Js.Result.Ok(v) => f(v)
  | Error(a) => Js.Result.Error(a)
});

let file = (~debug=?, ~includePaths=?, filePath) => {
  if ( ! Node.Fs.existsSync(filePath) ) {
    raise(AeroCssError("[AeroLess] File does not exist: " ++ filePath))
  };

  /*let debug' = Option.getWithDefault(debug, AeroConfig.dev);*/

  /* Will eventually need & implement this */
  /*let includePaths' = Option.getWithDefault(includePaths, [filePath])
    |> List.append(_, ["node_modules"])
    |> Array.of_list;*/

  let cache = ref(Cold);

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
      | Loading(futureCss) => futureCss |. Future.map(send) |> async
      | Cold =>
        let futureCss = bundle(filePath)
        |. bufferStreamtoFuture
        |. FutureResult.mapOk(buf => Js.Result.Ok(buf |> Buffer.toStringWithEncoding(_,"utf8")))
        |. FutureResult.tapOk(css => if (AeroConfig.prod) {
          cache := Warm(css)
        });

        cache := Loading(futureCss);
        futureCss |. Future.map(send) |. async
    }
  }
};
