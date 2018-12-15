# Peerify
A WIP implementation of a Peer to Peer File Sharing Network in C++i

Currently: Able to obtain clients which have file pieces, and able to connect to these clients over multiple threads to bring in the file pieces. However, there's a bug in which the last part of the image is not obtained. This is being investigated

It is in a very early development stage. The aim is to create a Peer to Peer file sharing network that works on the following basis:

1. The Server is just a bookkeeper that keeps a track of which clients are there on the network, and what pieces of files they have. No file is actually stored on the main server

2. The Clients are simultaneously clients and servers. In this model, the client can both send and recieve pieces from others on the network.


Intended features(Not yet added in):

1. Server distributes pieces to files on a demand basis, and keeps hashes to ensure security
2. Server ensures that all file pieces are available on the Network, and keeps multiple copies across multiple clients


Depends on BSD sockets
