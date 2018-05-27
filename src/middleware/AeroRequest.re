module Stream = NodeExtHttp.NodeExtStream.ReadableStream;
module Option = Belt.Option;
module Result = Belt.Result;
exception RequestError(string);

type bodyTypeParam =
  | Json(Js.t({.}))
  | JsonRaw(string)
  | UrlEncoded(Js.t({.}))
  | UrlEncodedRaw(string)
  | PlainText(string);

type bodyType =
  | BodyEmpty
  | BodyString(string)
  /*| BodyStream /*TODO*/*/;

type options = {
  method_: string,
  url: string,
  headers: Js.Dict.t(string),
  body: bodyType,
  timeout: option(int),
};

type response = {
  status:int,
  headers: Js.Dict.t(string),
  body: string,
};

let make = (method_, url) => {
  url: url,
  method_: method_,
  headers: Js.Dict.empty(),
  body: BodyEmpty,
  timeout: None,
};

let get = make("GET");
let post = make("POST");
let put = make("PUT");
let patch = make("PATCH");
let delete = make("DELETE");

let body = (contentType, content, opts : options) => {
  opts.headers |> Js.Dict.set(_, "content-type", contentType);
  { ...opts, body: content }
};

let bodyJson = (content) => body("application/json", BodyString(content));
let bodyUrlEncoded = (content, opts) => body("application/x-www-form-urlencoded", BodyString(content), opts);
let bodyText = (content, opts) => body("text/plain", BodyString(content), opts);

let header = (k, v, opts) => {
  opts.headers |> Js.Dict.set(_, k, v);
  opts
};

let headers = (hs, opts) => { ...opts, headers: hs };
let timeout = (ms, opts) => { ...opts, timeout: Some(ms) };

let go = (resolve) => (res) => {
  let body = ref("");
  res
  |> Stream.setEncoding("utf8")
  |> Stream.on(`data(chunk => {
    body := body^ ++ NodeExtStreamCast.unsafeDataOutputToString(chunk);
  }))
  |> Stream.onEnd(() => {
    let headers = res |. NodeExtHttp.IncomingMessage.getHeaders;
    let status = res |. NodeExtHttp.IncomingMessage.getStatusCode;
    { status, headers, body: body^ } |. Result.Ok |. resolve
  })
};

let run = (opts) => {
  let parts = opts.url |> NodeUrl.parseExn;
  let isHttps = parts##protocol == "https:";

  let node_req_opts = NodeExtHttp.HttpRequestOpts.t(
    ~hostname=parts##hostname,
    ~port=(parts##port == "" ? (isHttps ? 443 : 80) : int_of_string(parts##port)),
    ~path=parts##pathname,
    ~httpMethod=opts.method_,
    ~headers=opts.headers,
    ()
  );
  Future.make(resolve => {
    let req = isHttps
      ? NodeExtHttp.requestHttps(node_req_opts, ~callback=go(resolve), ())
      : NodeExtHttp.request(node_req_opts, ~callback=go(resolve), ())
    ;
    req
    |> NodeExtHttp.ClientRequest.on(`error(error => {
        /* TODO: Robust error handling */
        Js.log("Request error:");
        Js.log(error);
      }))
    |> (req => switch(opts.body) {
      | BodyString(content) =>
        NodeExtHttp.NodeExtStream.WritableStream.writeString(req, ~data=content, ()) |> ignore;
        req
      | BodyEmpty => req
    })
    |> NodeExtHttp.ClientRequest.end_
  })
};
