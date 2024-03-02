Assumptions:

I made an assumption that 3rd argument is the ONLY argument that contributes to setting the player name. Any command to execute client with more than 3 arguments errors and exits. It was unclear from the REQUIREMENTS.md whether or not EVERYTHING typed after the port number was supposed to be in the name. We asked our learning fellow, who said that only the fourth argument should be the name. We also think that this is the best design choice as it is simple and abides by the the unix standard. 

Beyond this, the only other assumption is that the server and the client share a mutually-understood communication protocol.  