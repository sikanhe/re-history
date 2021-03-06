type historyType =
  | Browser
  | Hash
  | Memory;

type action =
  | Pop
  | Push
  | Replace;

type location 'a = {
  search: string,
  pathname: string,
  hash: string,
  key: string,
  state: 'a
};

type blockerReturn =
  | Prompt string
  | Block
  | Pass;

type t 'a = {
  historyType: historyType,
  mutable length: int,
  mutable action: action,
  mutable location: location 'a,
  mutable listeners: list (action => location 'a => unit),
  mutable blockers: list (action => location 'a => blockerReturn),
  keyLength: int,
  forceRefresh: bool
};

let createLocationFromDOM key state=> {
  let browserLocation = Browser.location;
  {
    pathname: browserLocation##pathname,
    hash: browserLocation##hash,
    search: browserLocation##search,
    key,
    state
  };
};

let createLocation path key state => {
  let {Utils.pathname, search, hash} = Utils.parsePath path;
  {key, pathname, hash, search, state};
};

let createHref = fun
| {pathname, search: "", hash: ""} => pathname
| {pathname, search, hash: ""} => pathname ^ search
| {pathname, hash, search: ""} => pathname ^ hash
| {pathname, hash, search} => pathname ^ search ^ hash;

let createKey ::length=6 () => {
  let alpha = "1234567890abcdefghijklmnopqrstuvwxyz";
  let key = ref "";
  let bound = String.length alpha;
  for _n in 0 to length {
    let rand = Random.int bound;
    key := key.contents ^ (String.sub alpha rand 1);
  };
  key.contents
};

let unsubscribe history listener () => {
  history.listeners = List.filter (fun l => l !== listener) history.listeners;
};

let subscribe history listener => {
  unsubscribe history listener ();
  history.listeners = [listener, ...history.listeners];
  unsubscribe history listener;
};

let notifyListeners listeners action location => {
  List.iter (fun listener => listener action location) listeners;
};

let unblock history blocker () => {
  history.blockers = List.filter (fun bl => bl !== blocker) history.blockers;
};

let block history blocker => {
  unblock history blocker ();
  history.blockers = [blocker, ...history.blockers];
  unblock history blocker;
};

let rec checkWithBlockers blockers action location => {
  switch blockers {
    | [] => true
    | [blocker, ...rest] => {
      let didThisPass =
        switch (blocker action location) {
          | Pass => true
          | Block => false
          | Prompt message => Browser.confirm message;
        };

      didThisPass && (checkWithBlockers rest action location)
    }
  };
};

let go n => Browser.History.go n;
let goBack () => go (-1);
let goForward () => go 1;

 let handlePopEvent history popEvent => {
   /* Event should always have a state or else it is extraneous */
   switch (Js.Null_undefined.to_opt popEvent##state) {
     | None => ()
     | Some state => {
        let action = Pop;
        let location = createLocationFromDOM state##key state##state;

        if (checkWithBlockers history.blockers action location) {
          history.location = location;
          history.action = action;
          notifyListeners history.listeners action location;
        } else {
          let oldHref = createHref history.location;
          Browser.History.replaceState state Js.Null.empty oldHref;
        }
     }
   };
};

let change ::forceRefresh=false history action path state=> {
  let { keyLength, listeners, blockers} = history;
  let key = createKey length::keyLength ();
  let newLocation = createLocation path key state;
  let shouldRefresh = forceRefresh || history.forceRefresh;
  let shouldContinue = checkWithBlockers blockers action newLocation;

  switch (shouldContinue, action, shouldRefresh) {
    | (true, action, false) => {
      let browserHistory = Browser.history;

      notifyListeners listeners action newLocation;
      Browser.History.pushState {"key": key, "state": state} Js.Null.empty path;
      history.action = action;
      history.length = Browser.History.getLength browserHistory;
      history.location = newLocation;
    }
    | (true, Replace, _) => {
      Browser.Location.replace Browser.location path;
    }
    | (true, Push, _) => {
      Browser.Location.setHref Browser.location path;
    }
    | _ => ()
  };
};

let push ::forceRefresh=false ::state=? history path =>
  change history Push path state ::forceRefresh;

let replace ::forceRefresh=false ::state=? history path =>
  change history Replace path state ::forceRefresh;

let createBrowserHistory ::keyLength=8 ::forceRefresh=false () => {
  let key = createKey length::keyLength ();
  let initLocation = createLocationFromDOM key None;

  /* Set initial history.state  */
  Browser.History.replaceState {"key": key, "state": None} Js.Null.empty Browser.location##href;

  let history = {
    historyType: Browser,
    length: 0,
    action: Pop,
    location: initLocation,
    listeners: [],
    blockers: [],
    keyLength,
    forceRefresh,
  };

  Browser.onPopState (handlePopEvent history);

  history;
};