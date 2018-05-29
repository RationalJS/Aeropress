
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


[@bs.send]
external start : (server, 'a => 'b) => unit = "";
[@bs.send]
external stop : (server, 'a => 'b) => unit = "";


[@bs.send]
external on : (server, Js.t({..})) => server = "";
