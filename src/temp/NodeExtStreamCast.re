open NodeExtHttp.NodeExtStream;

external toBuffer : 'a => BsNode.NodeBuffer.t = "%identity";

type bufferClassification =
  | Buf(BsNode.NodeBuffer.t)
  | Str(string)
  | JsType(Js.Types.tagged_t);

let classifyDataOutput = (bufOrStr: dataOut) : bufferClassification =>
  if (Node_buffer.isBuffer(bufOrStr)) {
    Buf(toBuffer(bufOrStr));
  } else {
    let classified = Js.Types.classify(bufOrStr);
    switch (classified) {
    | Js.Types.JSString(str) => Str(str)
    | _ => JsType(classified)
    };
  };

let unsafeDataOutputToString = (bufOrStr: dataOut) =>
  Js.String.make(bufOrStr);
