open BsOspec.Cjs;
open AeroRoutes.Router;

open TestHelper;

describe("AeroLess", () => {

  testAsync("file", done_ => {
    let router = route
      &&& get("/assets/style.css")
      &&& AeroLess.file("fixtures/less/index.less" |> getTestPath);

    makeReq("GET", "/assets/style.css")
    |> router
    |> getRes(
      expectBody(~headers={ "content-type": "text/css" }, "body { color: red; }\n\n")
      >>% done_
    );
  });

});
