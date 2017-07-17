type action = 
  | POP 
  | PUSH
  | REPLACE;

type location = {
  search: string,
  pathname: string,
  hash: string,
  key: string
};
  
type blockerReturn = 
  | Prompt string 
  | Block
  | Skip;  
  
type t = {
  mutable length: int,
  mutable action: action,
  mutable location: location,
  mutable listeners: list (action => location => unit),  
  mutable blockers: list (action => location => blockerReturn),
  keyLength: int,
  forceRefresh: bool
};

let actionToString = fun
| POP => "POP"
| PUSH => "PUSH"
| REPLACE => "REPLACE";

let getDomLocation state => {
  let browserLocation = Browser.location;
  {
    pathname: browserLocation##pathname,
    hash: browserLocation##hash,
    search: browserLocation##search,
    key: state##key
  };
};

let createLocation path key => {
  let {Utils.pathname, search, hash} = Utils.parsePath path;
  {key, pathname, hash, search};
};

let createHref = fun
| {pathname, search: "", hash: ""} => pathname
| {pathname, search, hash: ""} => pathname ^ search 
| {pathname, hash, search: ""} => pathname ^ hash 
| {pathname, hash, search} => pathname ^ search ^ hash;

let createKey ::length=8 () => {
  let alpha = "1234567890abcdefghijk";
  let key = ref ""; 
  let bound = String.length alpha;
  for _n in 0 to length {
    let rand = Random.int bound;
    key := !key ^ (String.sub alpha rand 1);
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
  history.blockers = [blocker, ...history.blockers]; 
  unblock history blocker;
}; 

let rec checkWithBlockers blockers action location => {
  switch blockers {
    | [] => true 
    | [blocker, ...rest] => {
      let didThisPass = 
        switch (blocker action location) {
          | Skip => true
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

let handlePopEvent history (event: Js.t{. 
  state: Js.null(Js.t{. 
    key: string, 
    state: Js.t{..}
  })
}) => {
  switch (Js.Null.to_opt event##state) {
  | None => ()
  | Some state => {
    let action = POP;
    let key = state##key; 
    let state = state##state; 
    let location = getDomLocation({"key": key, "state": state}); 
    history.location = location; 
    history.action = action; 
    notifyListeners history.listeners action location;
  }
  };
};

let change ::forceRefresh=false ::state history action path => {
  let { keyLength, listeners, blockers } = history;
  let key = createKey length::keyLength ();
  let newLocation = createLocation path key;

  let shouldContinue = checkWithBlockers blockers action newLocation;
  switch (shouldContinue, action, forceRefresh, history.forceRefresh) {
    | (true, action, false, false) => { 
      let browserHistory = Browser.history;
      let state = switch state {
      | None => (Browser.History.getState browserHistory)##state
      | Some state => state
      }; 

      notifyListeners listeners action newLocation;
      Browser.History.pushState {"key": key, "state": state} Js.Null.empty path;
      history.action = action;
      history.length = Browser.History.getLength browserHistory;
      history.location = newLocation;
    }
    | (true, REPLACE, _, _) => {
      Browser.Location.replace Browser.location path;
    }
    | (true, PUSH, _, _) => {
      Browser.Location.setHref Browser.location path;
    }
    | _ => ()
  };
};

let push ::forceRefresh=false ::state=? history path => 
  change history PUSH path ::state ::forceRefresh;

let replace ::forceRefresh=false ::state=? history path => 
  change history REPLACE path ::state ::forceRefresh;

let createBrowserHistory ::keyLength=8 ::forceRefresh=false () => {
  let key = createKey length::keyLength(); 
  let initLocation = getDomLocation {"key": key};

  /* Push in a initial state so handlepop does skip POP back to initial page */
  let initState = {"key": key, "state": Js.Null.empty};
  let initPath = Browser.location##href;
  Browser.History.replaceState initState Js.Null.empty initPath;  

  let history = {
    length: 0,
    action: POP,
    location: initLocation,
    listeners: [],
    blockers: [],
    keyLength, 
    forceRefresh,
  };

  Browser.onPopState (handlePopEvent history);

  history;
};