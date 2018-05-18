
type search_params;

type parts = { .
  "hash": string,
  "host": string,
  "hostname": string,
  "href": string,
  "origin": string,
  "password": string,
  "pathname": string,
  "port": string,
  "protocol": string,
  "search": string,
  "searchParams": search_params,
  "username": string,
};

[@bs.module "url"]
[@bs.new]
external parseExn : string => parts = "URL";
