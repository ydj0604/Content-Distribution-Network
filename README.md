# Content-Distribution-Network

![alt text](https://camo.githubusercontent.com/deaa7f4d92755300dbf9c4b045a25fbfa321c5c8/68747470733a2f2f6431623130626d6c76716162636f2e636c6f756466726f6e742e6e65742f6174746163682f693564337238326d3163303564702f686331686b6c723576377975392f6937626e756c78326565366e2f63733138387069632e706e67)

Main Components: Client Application, Origin Server, Meta Server, CDN Nodes (local servers for caching), File Storage System (Central Data Center)

**Origin Server**

Origin Server basically works like a bridge that all Client Apps' initial communications go through (except for the actual file transfer), so it will need to handle the large number of requests. Therefore, it is designed to have minimal computation within itself. It focuses on correctly, efficiently redirecting client requests to appropriate Meta Server if there are multiple Meta Servers. Moreover, Origin Server itself does not maintain any information but heavily relies on Meta Server for both storing and computing information required for the entire system to work. As Origin Server works as the initial point of contact for all Client Applications, Client Applications only need to be aware of the ways to interact with Origin Server, and stay detached from details of the entire system. Also, when there is only a single Meta Server, Origin Server can be deployed in the same machine as Meta Server in order to eliminate the network delay for the communication between Meta Server and Origin Server.

**Meta Server**

Meta Server works close to Origin Server and functions as a brain of the entire system. Meta Server keeps track of a list of files that are uploaded or deleted, all the necessary meta data for every file, information about all the CDN Nodes in the system. 
In summary, Meta Server serves two main tasks: 
1) Maintain meta data (file hash, a list of CDN Nodes (cache nodes) that contain the file, timestamp, etc) about all the files stored in the system, 
2) Compile and send a list of files that Client Applications can use to make an appropriate request directly to CDN Nodes in order to transfer files

Selection Algorithm for choosing the CDN Node that will optimize the flow
1. CDN Nodes that have file(s) in cache already
2. Choose the geographically closest CDN Node
3. If all the CDN Nodes that have the file in cache are farther than FSS from the Client App, choose the closest CDN Node, which does not have the file in cache.
(this condition prevents the inefficiency which arises when a person first uploads a file in NY and later tries to access it in LA)

**CDN Node**

CDN Nodes are local cache nodes to transfer files from FSS to Client and vice versa. They will attempt to offload work from MetaServer by delivering content on their behalf. CDN Nodes will (1)transfer updated/created files to  FSS, (2) serve Client App's requests for files, (3) invalidate their cached files, (4) inform Meta Server with the up-to-date list of files they have. They will communicate with Client directly when they have to transfer the file. CDN caching is based upon LRU cache policy (Note that the terms CDN and CDN Node are used interchangeably)

Prefetching: in order to increase the possibility of cache-hits, CDN uses pre-fetching strategy. Whenever a request for a file comes into CDN, CDN fetches all the files that exist in the same directory from FSS. This is based on the assumption that every request for a file originates from Client Application, which never asks for just one specific file.

Cache Policy: LRU

**File Storage Server (FSS)**

The File Storage Server acts as our datacenter. Because our project focuses on the CDN and client layers, we designed our FSS implementation to be lightweight and only existing to store and serve files. Any sort of application logic should be at a higher level. The FSS implementation is simply a small server process that interfaces with the CDN nodes. It supports two operations, one to store a new file and one to serve files to requests. This simple interface creates a weak coupling between the CDN nodes and the FSS, allowing it to be easily scaled or expanded in the future. Features such as replication for load balancing or reliability could be added transparently, without having to touch other levels of our design. It could even be easily replaced by an existing solution like those discussed in class, such as NFS.

**Client Application**

The Client Application is a command line interface (CLI) that will be explicitly invoked by the user to perform syncing operations (split into two operations, download and upload). These processes operate in the same two-step manner, as described above. First a table is sent to find which files need to be updated. Then each file is either uploaded or downloaded (depending on the operation being performed). This minimizes the number of requests for files that are already up-to-date. Initial communication will occur through the Origin Node, but we have opted to have the Client Application also communicate with the CDN nodes directly for file transfers. The Client Application will be capable of listening for requests from the CDN nodes when downloading. To avoid multiple CDN’s trying to communicate with a single Client Application, the Client Application will only have a single file request outstanding at once. Finally, Authentication can be handled here by having the client encrypt and decrypt files before transferring them. Given a strong enough encryption scheme the files will only be readable by the client who uploaded them, and the encryption process should not be a bottleneck, especially when compared to the network transfer time.


Technologies used:

1. C++
2. casablanca (C++ REST framework): https://casablanca.codeplex.com/

Google Doc: https://docs.google.com/document/d/1vdE9084hsvU3WLp8chF8b2vXLoSgocfiD8iQCq24T-w/edit

Google PPT: https://docs.google.com/a/g.ucla.edu/presentation/d/1G8iBTZke0ICHwlI0blQ34HTgESA0paf6LCjBpROqnUs/edit?usp=sharing
