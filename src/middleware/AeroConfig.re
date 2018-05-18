
let nodeEnv = switch (Node.Process.process##env |> Js.Dict.get(_, "NODE_ENV")) {
  | Some(nodeEnv) => nodeEnv
  | None => "development" /* Default */
};

let dev = nodeEnv == "development";
let test = nodeEnv == "test";
let prod = nodeEnv == "production";

let env = (key, default) => switch (Node.Process.process##env |> Js.Dict.get(_, key)) {
  | Some(value) => value
  | None => default
};
