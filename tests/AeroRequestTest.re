open BsOspec.Cjs;
module MockServer = BsNodeMockHttpServer;

describe("AeroRequest", () => {

  let port = 9123;
  let server = MockServer.make(port);

  beforeEachAsync(done_ => {
    server |> MockServer.start(done_)
  });
  afterEachAsync(done_ => {
    server |> MockServer.stop(done_)
  });

  let echo = (bodyFn) => {
    "method": "POST",
    "path": "/echo",
    "reply": {
      "status": 200,
      "headers": [%raw "(req) => ({ 'content-type': req.headers['content-type'] })"],
      "body": bodyFn
    }
  };

  testAsync("get", done_ => {
    server
    |> MockServer.on({
        "method": "GET",
        "path": "/hello",
        "reply": {
          "status": 200,
          "headers": { "content-type": "application/json" },
          "body": TestHelper.stringify({ "hi": "there" })
        }
      }) |> ignore;

    AeroRequest.get("http://localhost:" ++ string_of_int(port) ++ "/hello")
    |> AeroRequest.run
    |. Future.get(result => switch(result) {
        | Belt.Result.Ok(body) =>
          body |> equals({json|{"hi":"there"}|json});
          done_()
        | Error(err) =>
          Js.log(err);
          raise(TestHelper.AssertionError("Request failed"))
      })
  });

  testAsync("post string", done_ => {
    server |> MockServer.on(echo([%raw "(req) => req.body"])) |> ignore;

    let payload = "hello?";
    AeroRequest.(
      post("http://localhost:" ++ string_of_int(port) ++ "/echo")
      |> bodyText(payload)
      |> run
    )
    |. Future.get(result => switch(result) {
        | Belt.Result.Ok(body) =>
          body |> equals(payload);
          done_()
        | Error(err) =>
          Js.log(err);
          raise(TestHelper.AssertionError("Request failed"))
      })
  });

  testAsync("post json", done_ => {
    server |> MockServer.on(echo([%raw "(req) => JSON.stringify(req.body)"])) |> ignore;

    /* Use wierdly formatted body to ensure server is parsing json per our header */
    let payload = "{  \"x\": 10,  \"y\":  [20, 30]}";
    let expected = Js.Json.stringifyAny({ "x": 10, "y": [|20,30|] }) |> Belt.Option.getExn;

    AeroRequest.(
      post("http://localhost:" ++ string_of_int(port) ++ "/echo")
      |> bodyJson(payload)
      |> run
    )
    |. Future.get(result => switch(result) {
        | Belt.Result.Ok(body) =>
          body |> equals(expected);
          done_()
        | Error(err) =>
          Js.log(err);
          raise(TestHelper.AssertionError("Request failed"))
      })
  });

});
