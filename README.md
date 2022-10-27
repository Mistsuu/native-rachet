# native-rachet

A project made for the submission of my undergraduate thesis, aiming to implement a Signal Protocol library that applies to my chat application *(link: https://github.com/Mistsuu/signalchat)*. This work is the result of following the documents of Moxie Marlinspike and Trevor Perrin *(link: https://signal.org/docs/)*. 

However, since the resulting application does not support **per-user identity key mode** to provide an in-person authentication method, it is still susceptible to the man-in-the-middle attack performed by the server. Also, this code is not optimized. Every arithmetic function is still performed through the `gmp` library, so its performance is still slow. It should be noticeable during the sending of the first message of each user.

This project is made for educational uses only.
