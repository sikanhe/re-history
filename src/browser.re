module Location = {
  type t = Js.t{. pathname: string, search: string, hash: string, href: string};
  /* This forces a refresh */
  external setHref: t => string => unit = "href" [@@bs.set];
  external replace: t => string => unit = "replace" [@@bs.send];
};

module History = {
  type t;
  external getLength: t => int = "length" [@@bs.get];
  external getState: t => Js.t{..} = "state" [@@bs.get];
  external setState: t => Js.t{..} => unit = "state" [@@bs.set];
  external pushState: Js.t{..} => Js.null string => string => unit = "window.history.pushState"
  [@@bs.val];
  external replaceState: Js.t{..} => Js.null string => string => unit = "window.history.pushState" 
  [@@bs.val];
  external go: int => unit = "window.history.go" 
  [@@bs.val];
};

type domNode;
external window: domNode = "window" [@@bs.val];
external history: History.t = "window.history" [@@bs.val];
external location: Location.t = "window.location" [@@bs.val];
external addEventListener: domNode => string => (Js.t {..} => unit) => unit 
  = "addEventListener" [@@bs.send];
let onPopState handler => addEventListener window "popstate" handler;
external confirm: string => bool = "window.confirm" [@@bs.val]; 