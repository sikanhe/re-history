# Why port to Reason instead of binding?

✔ Idiomatic Reason usage throughout

✔ Type checking for all function arguments/returns/options, and location descriptors.

✔ Take advantage of Variant types for action types (POP|PUSH|REPLACE) and
blocker functions (Prompt message | Block| Pass);

# TODO

- [x] push
- [x] replace
- [x] createBrowserHistory
- [x] listen/unlisten
- [x] block/unblock (for push/replace)
- [x] createHref
- [ ] block for POPs
- [ ] basename
- [ ] createHashHistory

# Example Usage

```reason
open History;

/* Example usages  */
type state = {mystate: string};

let history = createBrowserHistory ();
let unsub = subscribe history (fun action location => {
  Js.log (actionToString action);
  Js.log location.key;
  /* location.state is a variant of | Some state and | None  */
  switch location.state {
  | Some {mystate: state} => Js.log state;
  | None => Js.log "no state";
  }
});

/* callback function can return either | Prompt message | Block | Skip  */
let unblock = block history (fun action location => {
  Prompt ("You sure you want to" ^ actionToString(action) ^ "to " ^ createHref(location) ^ "?");
});

/* This will show a prompt before transitioning */
push history "/route1?query=yolo#someid" state::{mystate: "state1"};

unblock ();

push history "/route2#someid";
```