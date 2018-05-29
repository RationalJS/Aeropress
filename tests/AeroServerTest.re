open BsOspec.Cjs;
open TestHelper;
open Aeropress;

describe("AeroServer", () => {

  let port = 9124;
  let dangerousJs = [%raw () => "throw new Error('oops'); return 10"];

  testAsync("generic exception handling (sync)", done_ => {
    let router = route
      &&& get("/danger")
      &&& (r => {
        dangerousJs() |. ignore;
        AeroRoutes.Middleware.(r |. status(200) |. send(None))
      });

    let server = Server.create(router)
    |. Server.listen(port, "127.0.0.1", () => {
      Js.log2("[Test] Listening on port", port)
    });

    AeroRequest.(
      get("http://localhost:" ++ string_of_int(port) ++ "/danger")
      |. run
    )
    |. Future.get(result => switch(result) {
        | Belt.Result.Ok(res) =>
          res.status |. equals(500);
          res.body |. equals("oops");
          server |. Server.close(~callback=done_, ()) |. ignore
        | Error(err) =>
          Js.log(err);
          server |. Server.close(~callback=done_, ()) |. ignore;
          raise(AssertionError("Request failed"))
      })
  });

});

