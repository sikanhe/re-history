module Location = {
  type t = Js.t{. pathname: string, search: string, hash: string, href: string};
  /* This forces a refresh */
  external setHref: t => string => unit = "href" [@@bs.set];
  external replace: t => string => unit = "replace" [@@bs.send];
};

module History = {
  type t;
  type state 'a = Js.t{. key: string, state: 'a};

  external getLength: t => int = "length" [@@bs.get];
  external getState: t => state 'a = "state" [@@bs.get];
  external pushState: state 'a => Js.null string => string => unit = "window.history.pushState"
  [@@bs.val];
  external replaceState: state 'a => Js.null string => string => unit = "window.history.pushState"
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
external getUserAgentIndex: string => int = "navigator.userAgent.indexOf" [@@bs.val];