open AeroRoutes.Middleware;
module Option = Belt.Option;
module Dict = Js.Dict;

[@bs.module "cookie"]
external parse : string => Dict.t(string) = "parse";

let m = (r) => next_assign(r, {
  "cookies": Dict.get(r.req.headers, "cookie")
          |> Option.getWithDefault(_, "")
          |> parse
});
