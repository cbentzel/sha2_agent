Take the existing code, and change it to support a variety of hashes.

Rules
 - reference of sha256 as the generic program name should just be named hashgen
 - following hashes should be supported: sha1, sha256, sha512, md5, blake256, blake512
 - continue to use C++14, standard library only, and gtests
 - hash variant should be a command line flag
 - input continues to be stdio

General guidelines
 - prefer doing this with common stream handling class, and then a general hash interface class (e.g. pure virtuals) with derived concrete classes for each of the hash variants 