# carpark


Check == attempted  
✅ == complete  
~~cancelled~~

# Simulator
 -  Intalise memory (Check)
 -  Init other mutex (TODO)
 -  Generate a car 1-100ms (Check)
 -  Car logic (TODO)
    - ~~Add car queue (TODO)~~
    - Send car to entrance boom gate (Check)
    - Send license to LPR (Check)
    - Wait for manager to display info sign (check)
    - Wait for boomgate to open (check)
    - Drive to level {Trigger Level LPR} (check)
    - Park to set time (check)
    - Exit park on set level and trigger exit LPR (check)
    - Wait for exit boomgate (check)
    - Die (✅)

# Manager 
- Load shared memory (Check)
- Create LPR threads (Check)
    - Entrace LPR
        - Read plate of car (Check)
        - Display parking level on info sign (Check)
        - open entrance boom gate   (Check)
        - close entrance boom gate  (Check)
    - Level LPR
        - BRR idk trigger parked    (check)
    - Exit LPR
        - Reads plate   (Check)
        - Opens/Close boomgate (Check)
        - Free the parking spot (check)
        - Trigger billing (TODO:)
- Hash table
    - Check validity of plate (TODO)
    - Reject invalid car    (TODO)
- Manage carspaces 
    - Send cars to free parking levels (Check)
    - Reject car if full (Check)
- Billing 
    - Charge for each car   (TODO)
    - Total revenue (TODO)

- UI
    - Display info (Check)

    /**TODO:

    # BILLING
    # TEMP SENSE ON EACH LEVEL
    # HASH TABLE
    **/