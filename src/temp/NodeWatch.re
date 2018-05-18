type t;
type config;
[@bs.obj]
external config :
  (
    ~persistent: bool=?,
    ~recursive: bool=?,
    ~encoding: Js.String.t=?,
    unit
  ) =>
  config =
  "";
[@bs.module "fs"]
external watch : (string, ~config: config=?, unit) => t = "";
/** there is no need to accept listener, since we return a [watcher]
    back it can register event listener there .
    Currently we introduce a type [stringBuffer], for the
    [filename], it will be [Buffer] when the encoding is `utf8.
    This is dependent type which can be tracked by GADT in some way,
    but to make things simple, let's just introduce an or type
*/
[@bs.send]
external on :
  (
    t,
    [@bs.string] [
      | `change(
          (. string /*eventType*/, BsNode.Node.Buffer.t /* filename */) => unit,
        )
      | `error((. unit) => unit)
    ]
  ) =>
  t =
  "";
[@bs.send] external close : (t, unit) => unit = "";
