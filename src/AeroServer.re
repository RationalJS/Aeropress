module Req = NodeExtHttp.IncomingMessage;
module Res = NodeExtHttp.ServerResponse;
module WS = NodeExtHttp.NodeExtStream.WritableStream;
module Routes = AeroRoutes;

module RawHttp = {
  type req = Js.t({.
    headers: Js.Dict.t(string),
    url: string,
    _method: string,
  });

  type res = Js.t({.
    headers: Js.Dict.t(string),
    statusCode: int,
    [@bs.meth]
    writeHead: (int, Js.Dict.t(string)) => unit,
    [@bs.meth]
    _end: (string) => unit
  });

  type server = Js.t({.
    [@bs.meth]
    listen : (int, string, (. unit => unit)) => unit
  });

  type http = Js.t({.
    [@bs.meth]
    createServer : ((. req,res) => unit) => server
  });

  [@bs.module]
  external http : http = ""
};

let print_handler_action = (a) => Routes.(switch(a) {
  | Pass(_) => "Pass"
  | SendHeaders(_) => "SendHeaders"
  | Fail => "Fail"
  | Halt(_) => "Halt"
  | Async(_) => "Async"
});

let makeReq = (raw_req) => {
  Routes.headers: raw_req |. Req.getHeaders,
  url: raw_req |. Req.getUrl,
  method: raw_req |. Req.getMethod,
};

let makeRes = () => Routes.ResFresh(Routes.emptyHeaders());

let makeRouteContext = (raw_req) => {
  Routes.req: makeReq(raw_req),
  res: makeRes(),
  ctx: (Js.Obj.empty() : Routes.fresh_ctx),
  urlMatched: "",
};

/* Useful for testing */
let makeRouteContextLiteral = (method_, url, headers) => {
  Routes.req: { method: method_, url, headers },
  res: makeRes(),
  ctx: (Js.Obj.empty() : Routes.fresh_ctx),
  urlMatched: "",
};

let renderHeaders = (headers) => {
  let result = (Js.Dict.empty() : Js.Dict.t(string));
  Belt.Map.String.forEach(headers, (key, value) => {
    Js.Dict.set(result, key, value)
  });
  result
};

let writeServerError = (res) => {
  res |. Res.writeHead(500, ~headers=renderHeaders(Routes.emptyHeaders()), ());
  res |. WS.writeString("Internal server error", ()) |. ignore;
  res |. WS.endStream
}

let create = (routes) =>
  NodeExtHttp.createServer( (req, res) => {
    Js.log(Req.getMethod(req) ++ " " ++ Req.getUrl(req));
    let plan = try(
      (routes(makeRouteContext(req))
        : Routes.handler_action(Routes.endpoint, Js.t({.}), Routes.fresh, Routes.completed) )
    ) {
      | Js.Exn.Error(e) =>
        makeRouteContext(req)
        |. Routes.Middleware.status(500)
        |. Routes.Middleware.send(AeroConfig.prod
          ? Some("Interval Server Error")
          : e |. Js.Exn.message)
    };
    let rec execute = Routes.((p) =>
      switch (p) {
        | Halt({ res: ResEnded(headersSent, status_code, headers, body) }) =>
          if ( ! headersSent ) {
            res
            |. Res.writeHead(status_code, ~headers=renderHeaders(headers), ());
          };
          switch(body) {
            | Some(content) => res |. WS.writeString(~data=content, ()) |. ignore
            | None => ()
          };
          res |. WS.endStream
        | Halt({ res: ReqResStream(stream) }) =>
          req
          |. NodeExtHttp.NodeExtStream.ReadableStream.pipe(stream)
          |> NodeExtHttp.NodeExtStream.ReadableStream.on(`error(err => {
            Js.log2("Stream error:", err);
            writeServerError(res)
          }))
          |. NodeExtHttp.NodeExtStream.ReadableStream.pipe(res)
          |. ignore
        | Fail =>
          res |. Res.writeHead(404, ~headers=renderHeaders(emptyHeaders()), ());
          res |. WS.writeString("No such route: " ++ Req.getMethod(req) ++ " " ++ Req.getUrl(req), ()) |. ignore;
          res |. WS.endStream
        /*| Async(SendHeaders) => TODO */
        | Async(future) =>
          future |. Future.get(execute)
        | other =>
          Js.log("Invalid response: " ++ print_handler_action(other));
          writeServerError(res)
      }
    );
    execute(plan)
  });

let listen = (server, port, host, callback) => {
  server |. NodeExtHttp.NodeExtNet.Server.listenTCP(~port, ~host, ~callback, ());
  server
};

let close = NodeExtHttp.NodeExtNet.Server.close;
