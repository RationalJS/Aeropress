
/*
https://nodejs.org/docs/latest-v8.x/api/querystring.html#querystring_querystring_stringify_obj_sep_eq_options
*/
type stringifyOptions = {
  encodeURIComponent: (string) => string
};

[@bs.module "querystring"]
[@bs.val]
external stringify : Js.t({.}) => string = "";

[@bs.module "querystring"]
[@bs.val]
external stringifyCustom : (
  Js.t({.}),
  ~sep: string=?,
  ~eq: string=?,
  ~options: stringifyOptions=?,
  unit
) => string = "stringify";
