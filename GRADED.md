**Total: 90/100 points**

CANVAS final project grade xx % : SCORE*75% + Attendance*20% + Survey*5%

## (20) Design spec & Presentation
*18 out of 20 points:*

    -2 DESIGN.md calls for a `parseArgs` function, which does not exist.

## (15) Implementation Spec 
*13 out of 15 points:*
    -2 IMPLEMENTATION.md calls for a `parseArgs` function, which does not exist.

## (15) Code Style / Quality 
*12 out of 15 points, based on class style guide:*

    -2 `malloc` without test for failure:
    `server.c:139,146,175,322,394,598, ...`
    `player.c:193,223`
    -1 sometimes you do check for `malloc` failure, but you don't abort, you proceed anyway! See `server.c:361-370`

## (15) `git` Practices    
*15 out of 15 points:*

 * 6 git commit history - logical groupings of changes in each commit, and meaningful commit message
 * 6 git commit history - good use of branches and git-flow.
 * 3 no executable files, object files, core dump files, editor backup files, or extraneous files existing in any directory.
    
## (35) Functionality  
*22 out of 35 points:*

    -2 no map file specified, server starts with `main.txt` hardcoded!
  * (2) server commandline, per spec
    -1 doesn't use Seed argument or detect non-numeric Seed
  * (4) server supports one player, per spec
    -3 does not implement spec - it should work with my client, but it does not. Display seems to be missing '\n's.
  * (4) server supports multiple players, per spec
  * (2) server supports spectator, per spec
  * (4) server supports 'visibility' spec
  * (2) server tracks gold, per spec
    -1 spectator shows no more, display says 192 unclaimed BECAUSE gold that is collected while SPRINTING is disappearing while not being counted.
  * (1) server produces Game Over summary, per spec
  * (1) new, valid, non-trivial mapfile
    Yes, a really big one! Too bad I can't use your code to see it

  * (2) client commandline, per spec
    -1 does not detect nuon-numeric port or too many args
  * (4) client plays as player, per spec
    -3 does not implement spec - it should work with my server, but it does not
  * (4) client plays as spectator, per spec
    -2 does not implement spec - it should work with my server, but it does not
  * (4) client asks for window to grow, per spec
  * (1) client prints Game Over summary, per spec

### Special Penalties-

## Extra Credit+
10 points

5/8 Scrum (up to 8): 
0/5 Gold Stealing (up to 5):
5/5 Range Limit of Vision (up to 5):
0/5 Alert Players (up to 5):

