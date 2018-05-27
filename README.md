# Aeropress

Aeropress is a node server framework for ReasonML. It intends to be a typesafe version of the popular express.js framework.

## Installation

```
$ npm install @aeropress/server
```

Then add `"aeropress"` to your `bsconfig.json` dependencies:

```
{
  ...
  "bs-dependencies": [
    "aeropress"
  ]
}
```

## Basic Usage

Aeropress is still in early stages. Lots of features coming soon! With that said, the following should work:

```js
open Aeropress;

let getPath = BsNode.NodePath.join2(BsNode.NodeGlobal.__dirname);

let appRouter

  = route
&&& get("/assets/style.css")
&&& Less.file("../client/css/style.less" |> getPath)

||| route
&&& prefix("/assets")
&&& Static.dir("../client/build" |> getPath)

||| route
&&& get("/") &&& Static.file("../client/index.html" |> getPath);


let port = AeroConfig.env("PORT", "7272") |> int_of_string;
let hostname = AeroConfig.env("HOST", "127.0.0.1");

HttpServer.create(appRouter)
|> HttpServer.listen(port, hostname, () => {
  Js.log2("Listening on port", port)
});

```

## TODO

- [ ] Implement response streaming
  - [ ] Update AeroCss to use streaming

## Build
```
npm run build
```

## Build + Watch

```
npm run start
```

## Editor
If you use `vscode`, Press `Windows + Shift + B` it will build automatically
