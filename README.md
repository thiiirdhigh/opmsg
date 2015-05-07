opmsg
=====

A gpg alternative
-----------------

_opmsg_ is a replacement for _gpg_ which can encrypt/sign/verify
your mails or create/verify detached signatures of local files.
Even though the _opmsg_ output looks similar, the concept is entirely
different.

Personas
--------

The key concept of _opmsg_ is the use of personas. personas are
an identity with a RSA key bound to it. Communication happens between
two personas (which could be the same) which are uniquely indentified
by the hashsum of their RSA keys:

```
$ opmsg --newp --name stealth

opmsg: version=1 -- (C) 2015 opmsg-team: https://github.com/stealth/opmsg

opmsg: creating new persona

.......[...].........................o..o..o..o..o..oO

opmsg: Successfully generated persona (RSA + DHparams) with id
opmsg: 1cb7992f96663853 1d33e59e83cd0542 95fb8016e5d9e35f b409630694571aba
opmsg: Tell your remote peer to add the following RSA pubkey like this:
opmsg: opmsg --import --phash sha256 --name stealth

-----BEGIN PUBLIC KEY-----
MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQC4Xds/bPlkdqA9VhDBOIEV/Dc9
4EfL5aPBOQAdTIaZKE69SJdwakFhqOY1PeaeGRDcGTVNLBQ1Udgbc2YCgQh1X5Dn
veRIGJoGfqWC7zeq/mx6yRer3PTUOA0gr30Uu7IO128fVDxNLYYUuvzhzcdysZAa
WkmRflKuaCEMQ3RjcQIDAQAB
-----END PUBLIC KEY-----

opmsg: Check (by phone, otr, twitter, id-selfie etc.) that above id matches
opmsg: the import message from your peer.
opmsg: AFTER THAT, you can go ahead, safely exchanging op-messages.

opmsg: SUCCESS.
```

This is pretty self-explaining. A new persona with name `stealth` is
created. The public RSA key of this persona has to be imported by
the remote peer that you want to opmsg-mail with via

```
opmsg --import --phash sha256 --name stealth`
```

just as hinted above.

_opmsg_ does not rely on a web-of-trust which in fact never really
worked. Rather, due to ubicious messenging, its much simpler today
to verify the hashsum of the persona via **additional** communication
paths. E.g. if you send the pubkey via plain mail, use SMS **and** twitter
to distribute the hash, or send a picture/selfie with the hash
and something that uniquely identifies you. Using **two additional**
communication paths, which are unrelated to the path that
you sent the key along, you have a high degree of trust.

By default `sha256` is used to hash the RSA key (more precise the
**n** of the RSA public part), but you may also specify `ripemd160`
or `sha512`. Whichever you choose, its important that your peer knows
about it during import, because you will be referenced with this hex hash value
in future.

The private part of the keys which are stored inside `~/.opmsg`
are NOT encrypted. It is believed that once someone gained access
to your account, its all lost anyway (except for PFS as explained later),
so a passpharse just add a wrong feeling of security here. Keep
your account/box unpwned! Otherwise end2end encryption makes little
sense.

_opmsg_ encourages users for easy persona creation and throwaway.
The directory structure below `~/.opmsg` is easy and straight
forward. It just maps the hex ids of the personas and DH keys
to directories and can in fact be edited by hand.

Keys
----

Now for the coolest feature of _opmsg_: Perfect Forward Secrecy (PFS).

Without any need to re-code your mail clients or add bloat to the
SMTP protocol, _opmsg_ supports PFS by means of DH Kex out of the box.
That is, as op-messages are _always_ signed by its source persona,
whenever you send an opmsg to some other persona, a couple of
DH keys are generated and attached to the message. The remote
_opmsg_ will verify its integrity (as it has this persona imported)
and add it to this persona's keystore. So whenever this remote peer
sends you a mail next time, it can choose one of the DH keys it has
got beforehand. If your peer runs out of DH keys, _opmsg_ falls
back to RSA encryption. The peer deletes used DH pubkeys to not
use them twice and the local peer marks used keys with a
`used` file within the apropriate key-directory. Once again,
`sha256` is used by default to index and to (worldwide) uniquely
identify DH keys.

**Attention:** If you keep encrypted op-messages in your mailbox,
do not throw away this persona. You wont be able to decrypt these mails
afterwards! Throwing away a persona also means to throw away all keying
material. Thats why _opmsg_ has no switch to erase personas. You have
to do it by hand, by rm-ing the subdirectory of your choice. Thats
easily done, but keep in mind that any dangling op-messages in your
inbox will become unreadable, as all keys will be lost. If you want to
benefit from PFS, you have to archive the **decrypted** messages and
throw away `used` keys. After all _opmsg_ is not a crypto container
or a replacement for FDE (which is recommended anyway). _opmsg_ is
about to protect your messages in transit, not on disk.



mutt integration
----------------

Just add to your _.muttrc_:

```
# Add a header so to easy pick opmsg via procmail rules
my_hdr X-opmsg: version1

set pgp_long_ids

set pgp_list_pubring_command="/usr/local/bin/opmsg --listpgp --short"
set pgp_encrypt_sign_command="/usr/local/bin/opmsg --encrypt %r -i %f"
set pgp_encrypt_only_command="/usr/local/bin/opmsg --encrypt %r -i %f"
set pgp_decrypt_command="/usr/local/bin/opmsg --decrypt -i %f"
set pgp_verify_command="/usr/local/bin/opmsg --decrypt -i %f"
```

and work with your mails as you would it with PGP/GPG before. If you
use a mix of GPG and _opmsg_ peers, its probably wise to create
a dedicated _.muttrc_ file for _opmsg_ and route _opmsg_ mails to
a different inbox, so you can easily work with GPG and _opmsg_ in
parallel.
