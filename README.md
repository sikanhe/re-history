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

type state = {mystate: string};

let history = createBrowserHistory ();

let unsub = subscribe history (fun action location => {
  Js.log (actionToString action);
  Js.log location.key;
  Js.log (Browser.History.getState Browser.history);
});

let unblock = block history (fun action location => {
  /* Return `Prompt message` variant to show a `window.confirm` prompt */
  Prompt ("You sure you want to" ^ actionToString(action) ^ "to " ^ createHref(location) ^ "?");
  /* or just block this transition */
  Block
  /* or let it pass */
  Pass
});

/* This will show a prompt before transitioning */
push history "/route1?query=yolo#someid";

unblock ();

/* Can also pass in a state JS object*/
push history "/route2#someid" state::{mystate: "second state"};
```
/* explicitly set the state to null */
push history "/route2#someid"