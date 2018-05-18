open AeroRoutes;
open AeroRoutes.Middleware;
module Path = BsNode.NodePath;
module Str = Js.String;

[@bs.module]
external filed : string => Js.t(#NodeExtHttp.NodeExtStream.duplexStream) = "filed";

let log = (x,y) => { Js.log2(x,y); y };

let dir = (root) => (r) => {
  Js.log4("Matched so far:", r.urlMatched, "|", r.req.url);
  Js.log2("root",root);
  let file = r.req.url
  |> Str.sliceToEnd(~from=Str.length(r.urlMatched))
  |> Path.join2(root);

  if (Str.startsWith(root, file)) {
    Js.log2("> Serving", file);
    r |> sendStream(filed(file))
  }
  else {
    Js.log2("> No such file:", file);
    /* Prevent directory traversal attack */
    r |> status(404) |> sendText("Not found")
  }
};

let file = (path) => (r) =>
  r |> sendStream(filed(path));
