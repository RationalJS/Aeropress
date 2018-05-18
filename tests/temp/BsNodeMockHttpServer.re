
type server;

type server_options = { .
  "host": string,
  "port": int,
};

[@bs.module]
[@bs.new]
external _make : (server_options) => server = "mock-http-server";

let make = (~host="localhost", port : int) =>
  _make({ "host": host, "port": port });


[@bs.send.pipe: server]
external start : ('a => 'b) => unit = "";
[@bs.send.pipe: server]
external stop : ('a => 'b) => unit = "";


[@bs.send.pipe: server]
external on : (Js.t({..})) => server = "";
