### Research work: "Parallel programming technologies in Linux operating systems"


The object of the study is an OS based on the Linux kernel, namely Debian,
and parallel programming technologies in this environment.

The purpose of this study is to study the operating system
based on the Linux kernel, specifically Debian, as well as to study
parallel programming technologies in this environment.

The main objective of the project is to transfer the application from
the graph theory laboratory, where messages
from a certain alphabet are encoded and decoded using the Huffman algorithm. Program
It must use MPI technology, be assembled and launched on one of
the nodes, and in the process use the computing resources of three nodes (
including the launching node and the other two). All nodes must run on
a Linux operating system with a Debian distribution.


To achieve this goal, the following steps were completed: installation and
configuration of the VirtualBox virtual environment, installation of the operating system and basic
network configuration, study of the main possibilities of interaction with Linux OS,
preparation of the development environment and transfer of the application from
the graph theory laboratory work.


As a result of the project, code using MPI technology was written. The
output message for encoding is divided into equal parts, which are processed
by several processes, which speeds up the program, since
data encoding occurs in parallel on each process.


The main advantages and characteristics of the project include an increase
in data transfer speed (maximum data transfer rate in the network) and
network throughput (maximum amount of data transmitted
through the network over a certain period of time) through the use of parallel-computer programming. Network reliability is ensured by connecting
three nodes to a local network, and data exchange is secure due
to the configuration of an SSH server for each node.


The final application demonstrates efficiency in increasing
the data transfer rate and network bandwidth, as well as ensuring
the safety and reliability of the network. In addition, it can be scaled by
increasing the number of nodes.



## Documentation in Russian Language: [here](https://github.com/MikhailCherepanovD/SammerPractis/blob/main/Documentation.pdf)





