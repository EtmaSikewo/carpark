# carpark


Check == attempted  
✅ == complete  
~~cancelled~~

# Simulator
 -  Intalise memory (✅)
 -  Init other mutex (✅)
 -  Generate a car 1-100ms (✅)
 -  Car logic (check)
    - ~~Add car queue (TODO)~~
    - Send car to entrance boom gate (✅)
    - Send license to LPR (✅)
    - Wait for manager to display info sign (✅)
    - Wait for boomgate to open (✅)
    - Drive to level {Trigger Level LPR} (✅)
    - Park to set time (✅)
    - Exit park on set level and trigger exit LPR (✅)
    - Wait for exit boomgate (✅)
    - Die (✅)

# Manager 
- Load shared memory (✅)
- Create LPR threads (✅)
    - Entrace LPR
        - Read plate of car (✅)
        - Display parking level on info sign (✅)
        - open entrance boom gate   (✅)
        - close entrance boom gate  (✅)
    - Level LPR
        - BRR idk trigger parked    (✅)
    - Exit LPR
        - Reads plate   (✅)
        - Opens/Close boomgate (✅)
        - Free the parking spot (✅)
        - Trigger billing (✅)
- Hash table
    - Check validity of plate (TODO)
    - Reject invalid car    (TODO)
- Manage carspaces 
    - Send cars to free parking levels (✅)
    - Reject car if full (✅)
- Billing 
    - Charge for each car   (✅)
    - Total revenue (✅)

- UI
    - Display info (✅)



    #TODO HASH TABLE

    #TODO MAKE VIDEO