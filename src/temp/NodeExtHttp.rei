/*
 * TODO: Replace with bs-node-ext dependency
 */
module NodeExtStream: {
  type dataOut;

  type streamType(_) =
    | Buffer : streamType(BsNode.Node.Buffer.t)
    | String : streamType(string);

  /*
   Ensure each class is structrually different.
   */
  type writableStreamClass;

  class type writableStream =
    [@bs]
    {
      pub classWritableStream: writableStreamClass
    };

  /*
   Ensure each class is structrually different.
   */
  type readableStreamClass;

  class type readableStream =
    [@bs]
    {
      pub classReadableStream: readableStreamClass
    };

  type duplexStreamClass;

  class type duplexStream =
    [@bs]
    {
      inherit readableStream;
      inherit writableStream;
      pub classDuplexStream: duplexStreamClass
    };

  type transformStreamClass;

  class type transformStream =
    [@bs]
    {
      inherit duplexStream;
      pub classTransformStream: transformStreamClass
    };

  type passThroughClass;

  class type passThroughStream =
    [@bs]
    {
      inherit transformStream;
      pub classPassThroughStream: passThroughClass
    };

  module ReadableStream: {
    [@bs.send] external isPaused : Js.t(#readableStream) => bool = "";
    [@bs.send.pipe: Js.t(#readableStream)]
    external pause : unit => Js.t(#readableStream) = "";
    [@bs.send]
    external pipe :
      (Js.t(#readableStream), Js.t(#writableStream)) => Js.t(#readableStream) =
      "";
    [@bs.get]
    external getReadableHighWaterMark : Js.t(#readableStream) => int =
      "readableHighWaterMark";
    [@bs.set]
    external setReadableHighWaterMark : (Js.t(#readableStream), int) => int =
      "readableHighWaterMark";
    [@bs.send] external read : (Js.t(#readableStream), int) => dataOut = "";
    [@bs.get]
    external getReadableLength : Js.t(#readableStream) => int =
      "readableLength";
    [@bs.send.pipe: Js.t(#readableStream)]
    external resume : unit => Js.t(#readableStream) = "";
    [@bs.send.pipe: Js.t(#readableStream)]
    external setEncoding : string => Js.t(#readableStream) = "";
    [@bs.send]
    external unpipe :
      (Js.t(#readableStream), Js.t(#writableStream), unit) =>
      Js.t(readableStream) =
      "";
    [@bs.send] external unpipeAll : Js.t(#readableStream) => unit = "unpipe";
    [@bs.send.pipe: Js.t(#readableStream)]
    external on :
      (
      [@bs.string]
      [
        | `close(unit => 'a)
        | `data(dataOut => 'a)
        | `error(Js.Exn.t => unit)
        | `readable(unit => unit)
      ]
      ) =>
      Js.t(#readableStream) =
      "";
    [@bs.send.pipe: Js.t(#readableStream)]
    external onEnd : ([@bs.as "end"] _, unit => unit) => 'a = "on";

    [@bs.send.pipe: Js.t(#readableStream)]
    external onData :
      (
        [@bs.ignore] streamType('a),
        [@bs.as "data"] _,
        'a => 'b
      ) => Js.t(#readableStream) =
      "on";

    [@bs.send.pipe: Js.t(#readableStream)]
    external onFinish : ([@bs.as "finish"] _, Js.Nullable.t(Js.Exn.t) => unit) => 'a = "on";
  };

  module WritableStream: {
    [@bs.send] external cork : Js.t(#writableStream) => unit = "";
    [@bs.send] external endStream : Js.t(#writableStream) => unit = "end";
    [@bs.send] external uncork : Js.t(#writableStream) => unit = "";
    [@bs.get]
    external getWritableHighWaterMark : Js.t(#writableStream) => int =
      "writableHighWaterMark";
    [@bs.get]
    external getWritableLength : Js.t(#writableStream) => int =
      "writableLength";
    [@bs.send]
    external writeString :
      (
        Js.t(#writableStream),
        ~data: string,
        ~encoding: [@bs.string] [
                     | [@bs.as "ascii"] `ascii
                     | [@bs.as "base64"] `base64
                     | [@bs.as "binary"] `binary
                     | [@bs.as "hex"] `hex
                     | [@bs.as "ucs2"] `ucs2
                     | [@bs.as "utf16le"] `utf16le
                     | [@bs.as "utf8"] `utf8
                     | [@bs.as "latin1"] `latin1
                   ]
                     =?,
        unit
      ) =>
      bool =
      "write";
    [@bs.send]
    external writeBuffer :
      (
        Js.t(#writableStream),
        ~data: BsNode.NodeBuffer.t,
        ~encoding: [@bs.string] [
                     | [@bs.as "ascii"] `ascii
                     | [@bs.as "base64"] `base64
                     | [@bs.as "binary"] `binary
                     | [@bs.as "hex"] `hex
                     | [@bs.as "ucs2"] `ucs2
                     | [@bs.as "utf16le"] `utf16le
                     | [@bs.as "utf8"] `utf8
                     | [@bs.as "latin1"] `latin1
                   ]
                     =?,
        unit
      ) =>
      bool =
      "write";
    external destroy : (Js.t(#writableStream), Js.Exn.t) => unit = "";
    [@bs.send.pipe: Js.t(#writableStream)]
    external on :
      (
      [@bs.string]
      [
        | `close(unit => unit)
        | `drain(unit => unit)
        | `finish(unit => unit)
        | `pipe(Js.t(#readableStream) => unit)
        | `unpipe(Js.t(#readableStream) => unit)
      ]
      ) =>
      Js.t(#writableStream) =
      "";
    [@bs.send.pipe: Js.t(#writableStream)]
    external onEnd : ([@bs.as "end"] _, unit => unit) => 'a = "on";
  };

  [@bs.new] [@bs.module "stream"]
  external createPassThroughStream : unit => Js.t(passThroughStream) =
    "PassThrough";
};

module NodeExtNet: {
  type classServer;

  type classSocket;

  class type socket =
    [@bs]
    {
      inherit NodeExtStream.duplexStream;
      pub classSocket: classSocket
    };

  type address = {
    .
    "port": int,
    "family": string,
    "address": string,
  };

  class type server =
    [@bs]
    {
      pub classServer: classServer
    };

  module Server: {
    [@bs.send] external address : Js.t(#server) => address = "";
    [@bs.send]
    external close :
      (Js.t(#server), ~callback: unit => unit, unit) => Js.t(server) =
      "";
    [@bs.send]
    external getConnections :
      (Js.t(#server), (Js.undefined(Js.Exn.t), int) => unit) => unit =
      "";
    [@bs.send] external listen : Js.t(#server) => unit = "";
    [@bs.send]
    external listenTCP :
      (
        Js.t(#server),
        ~port: int,
        ~host: string=?,
        ~callback: unit => unit=?,
        unit
      ) =>
      unit =
      "listen";
    [@bs.send]
    external listenUnix :
      (
        Js.t(#server),
        ~path: string,
        ~backlog: int=?,
        ~callback: unit => unit=?,
        unit
      ) =>
      unit =
      "listen";
    [@bs.send] external getListening : Js.t(#server) => bool = "listening";
    [@bs.send]
    external getMaxConnections : Js.t(#server) => int = "maxConnections";
    [@bs.send] external ref : Js.t(#server) => Js.t(#server) = "";
    external unref : Js.t(#server) => Js.t(#server) = "";
    [@bs.send.pipe: Js.t(#server)]
    external on :
      (
      [@bs.string]
      [
        | `close(unit => unit)
        | `connection(socket => unit)
        | `error(Js.Exn.t => unit)
        | `listening(unit => unit)
      ]
      ) =>
      Js.t(#server) =
      "";
  };

  /**
   * TODO: Finish socket bindings.
   */
  module Socket: {
    include (module type of NodeExtStream.ReadableStream);
    include (module type of NodeExtStream.WritableStream);
    [@bs.send] external address : Js.t(#socket) => address = "";
    [@bs.send.pipe: Js.t(#socket)]
    external on :
      (
      [@bs.string]
      [
        | `close(unit => unit)
        | `connect(unit => unit)
        | `lookup(
            (
              Js.undefined(Js.Exn.t),
              Js.undefined(string),
              Js.undefined(string),
              Js.undefined(string)
            ) =>
            unit,
          )
        | `timeout(unit => unit)
      ]
      ) =>
      Js.t(#server) =
      "";
  };
};

open BsNode;

type socket = NodeExtNet.socket;

type classClientRequest;

class type clientRequest =
  [@bs]
  {
    inherit NodeExtStream.writableStream;
    pub classClientRequest: classClientRequest
  };

type classServer;

class type server = {
  inherit NodeExtNet.server;
  pub httpClassServer: classServer
};

/* TODO: inherit from net server */
type classServerResponse;

class type serverResponse =
  [@bs]
  {
    inherit NodeExtStream.writableStream;
    pub classServerResponse: classServerResponse
  };

type classIncomingMessage;

class type incomingMessage =
  [@bs]
  {
    inherit NodeExtStream.readableStream;
    pub classIncomingMessage: classIncomingMessage
  };

module ClientRequest: {
  type error;
  /* placeholder type */
  [@bs.send] external abort : Js.t(#clientRequest) => unit = "";
  [@bs.get]
  external getConnection : Js.t(#clientRequest) => socket = "connection";
  [@bs.send] external flushHeaders : Js.t(#clientRequest) => unit = "";
  [@bs.send]
  external getHeader : (Js.t(#clientRequest), string) => string = "";
  [@bs.send]
  external removeHeader : (Js.t(#clientRequest), string) => unit = "";
  external setHeader : (Js.t(#clientRequest), string, string) => unit = "";
  [@bs.send] external setNoDelay : (Js.t(#clientRequest), bool) => unit = "";
  [@bs.send.pipe: Js.t(#clientRequest)]
  external setTimeout : (Js.t(#clientRequest), int) => Js.t(#clientRequest) =
    "";
  external setTimeoutCb :
    (Js.t(#clientRequest), int, unit => unit) => Js.t(#clientRequest) =
    "setTimeout";
  [@bs.send.pipe: Js.t(#clientRequest)]
  external on :
    (
    [@bs.string]
    [
      | `abort(unit => unit)
      | `error(error => unit)
      | `connect((Js.t(incomingMessage), socket, NodeBuffer.t) => unit)
      | `continue(unit => unit)
      | `response(Js.t(incomingMessage) => unit)
      | `socket(socket => unit)
      | `upgrade((Js.t(incomingMessage), socket, NodeBuffer.t) => unit)
    ]
    ) =>
    Js.t(#clientRequest) =
    "";
  [@bs.send.pipe: Js.t(#clientRequest)]
  external end_ : unit = "end";
};

module Server: {
  include (module type of NodeExtNet.Server);
  [@bs.send] external close : Js.t(#server) => unit = "";
  [@bs.send] external closeCb : (unit => unit) => unit = "close";
  [@bs.send] external listen : Js.t(#server) => unit = "";
  [@bs.get] external getListening : Js.t(#server) => bool = "listening";
  [@bs.get]
  external getMaxHeadersCount : Js.t(#server) => int = "maxHeadersCount";
  [@bs.send]
  external setTimeout : (Js.t(#server), int) => Js.t(#clientRequest) = "";
  external setTimeoutCb :
    (Js.t(#server), int, unit => unit) => Js.t(#clientRequest) =
    "setTimeout";
  [@bs.get] external getTimeout : Js.t(#server) => int = "timeout";
  [@bs.get]
  external getKeepAliveTimeout : Js.t(#server) => int = "keepAliveTimeout";
  [@bs.set]
  external setKeepAliveTimeout : (Js.t(#server), int) => int =
    "keepAliveTimeout";
  [@bs.send.pipe: Js.t(server)]
  external on :
    (
    [@bs.string]
    [
      | `checkContinue(
          (Js.t(incomingMessage), Js.t(serverResponse)) => unit,
        )
      | `checkException(
          (Js.t(incomingMessage), Js.t(serverResponse)) => unit,
        )
      | `clientError((Js.Exn.t, socket) => unit)
      | `close(unit => unit)
      | `connection(socket => unit)
      | `request((Js.t(incomingMessage), Js.t(serverResponse)) => unit)
      | `upgrade((Js.t(incomingMessage), socket, NodeBuffer.t) => unit)
    ]
    ) =>
    Js.t(server) =
    "";
};

module ServerResponse: {
  include (module type of NodeExtStream.WritableStream);
  [@bs.send]
  external addTrailers : (Js.t(#serverResponse), Js.Dict.t(string)) => unit =
    "";
  [@bs.get]
  external getConnection : Js.t(#serverResponse) => socket = "connection";
  [@bs.get] external getFinished : Js.t(#serverResponse) => bool = "finished";
  [@bs.send]
  external getHeader : (Js.t(#serverResponse), string) => unit = "";
  [@bs.send]
  external getHeaderNames : Js.t(#serverResponse) => array(string) = "";
  [@bs.send]
  external getHeaders : Js.t(#serverResponse) => Js.Dict.t(string) = "";
  [@bs.send] external hasHeader : Js.t(#serverResponse) => bool = "";
  [@bs.get]
  external getHeadersSent : Js.t(#serverResponse) => bool = "headersSent";
  [@bs.send]
  external removeHeader : (Js.t(#serverResponse), string) => bool = "";
  [@bs.set]
  external setSendDate : (Js.t(#serverResponse), bool) => bool = "sendDate";
  [@bs.send]
  external setHeader : (Js.t(#serverResponse), string, string) => bool = "";
  external setTimeout : (Js.t(#serverResponse), int) => Js.t(#clientRequest) =
    "";
  external setTimeoutCb :
    (Js.t(#serverResponse), int, unit => unit) => Js.t(#clientRequest) =
    "setTimeout";
  [@bs.get] external getSocket : Js.t(#serverResponse) => socket = "socket";
  [@bs.get]
  external getStatusCode : Js.t(#serverResponse) => int = "statusCode";
  [@bs.set]
  external setStatusCode : (Js.t(#serverResponse), int) => int = "statusCode";
  [@bs.get]
  external getStatusMessage : Js.t(#serverResponse) => string =
    "statusMessage";
  [@bs.set]
  external setStatusMessage : (Js.t(#serverResponse), string) => string =
    "statusMessage";
  [@bs.send] external writeContinue : Js.t(#serverResponse) => unit = "";
  [@bs.send]
  external writeHead :
    (
      Js.t(#serverResponse),
      int,
      ~headers: Js.Dict.t(string)=?,
      unit
    ) =>
    unit =
    "";
  [@bs.send]
  external writeHeadWithStatusMessage :
    (
      Js.t(#serverResponse),
      int,
      string,
      ~headers: Js.Dict.t(string)=?,
      unit
    ) =>
    unit =
    "";
  [@bs.send.pipe: Js.t(#serverResponse)]
  external on :
    ([@bs.string] [ | `close(unit => unit) | `finish(unit => unit)]) =>
    Js.t(#serverResponse) =
    "";
};

module IncomingMessage: {
  [@bs.send] external destroy : Js.t(#incomingMessage) => unit = "";
  [@bs.send]
  external destroyWithError : (Js.t(#incomingMessage), Js.Exn.t) => unit = "";
  [@bs.get]
  external getHeaders : Js.t(#incomingMessage) => Js.Dict.t(string) =
    "headers";
  [@bs.get]
  external getHttpVersion : Js.t(#incomingMessage) => string = "httpVersion";
  [@bs.get] external getMethod : Js.t(#incomingMessage) => string = "method";
  [@bs.get]
  external getRawHeader : Js.t(#incomingMessage) => array(string) =
    "rawHeaders";
  [@bs.get]
  external getRawTrailers : Js.t(#incomingMessage) => array(string) =
    "rawTrailers";
  [@bs.get] external getSocket : Js.t(#incomingMessage) => socket = "socket";
  [@bs.get]
  external getStatusCode : Js.t(#incomingMessage) => int = "statusCode";
  [@bs.get]
  external getStatusMessage : Js.t(#incomingMessage) => socket =
    "statusMessage";
  [@bs.get]
  external getTrailers : Js.t(#incomingMessage) => socket = "trailers";
  [@bs.get] external getUrl : Js.t(#incomingMessage) => string = "url";
  [@bs.send.pipe: Js.t(#incomingMessage)]
  external on :
    ([@bs.string] [ | `aborted(unit => unit) | `close(unit => unit)]) =>
    Js.t(#incomingMessage) =
    "";
};

[@bs.module "http"]
external createServer :
  ((Js.t(incomingMessage), Js.t(serverResponse)) => unit) => Js.t(server) =
  "createServer";

[@bs.module "http"]
external getUrl :
  (string, Js.t(incomingMessage) => unit) => Js.t(clientRequest) =
  "get";

module HttpRequestOpts: {
  [@bs.deriving abstract]
  type t = {
    [@bs.optional]
    protocol: string,
    [@bs.optional]
    host: string,
    [@bs.optional]
    hostname: string,
    [@bs.optional]
    family: int,
    [@bs.optional]
    port: int,
    [@bs.optional]
    localAddress: string,
    [@bs.optional]
    socketPath: string,
    [@bs.optional] [@bs.as "method"]
    httpMethod: string,
    [@bs.optional]
    path: string,
    [@bs.optional]
    headers: Js.Dict.t(string),
    [@bs.optional]
    auth: string,
    [@bs.optional]
    timeout: int,
    [@bs.optional]
    setHost: bool,
  };
};

[@bs.module "http"]
external getWithOptions :
  (HttpRequestOpts.t, ~callback: Js.t(incomingMessage) => unit=?, unit) =>
  Js.t(clientRequest) =
  "get";

[@bs.module "http"]
external request :
  (HttpRequestOpts.t, ~callback: Js.t(incomingMessage) => unit=?, unit) =>
  Js.t(clientRequest) =
  "";

[@bs.module "https"]
external requestHttps :
  (HttpRequestOpts.t, ~callback: Js.t(incomingMessage) => unit=?, unit) =>
  Js.t(clientRequest) =
  "request";
