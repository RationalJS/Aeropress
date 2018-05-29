open BsOspec.Cjs;
open AeroRoutes.Router;

open TestHelper;

describe("AeroCss", () => {

  testAsync("file", done_ => {
    let router = route
      &&& get("/assets/style.css")
      &&& AeroCss.file("fixtures/css/index.css" |> getTestPath);

    makeReq("GET", "/assets/style.css")
    |> router
    |. getRes(
      expectBody(~headers={ "content-type": "text/css" }, "body { color: red; }\n;\n;")
      >>% done_
    );
  });

});
