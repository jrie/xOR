# xOR
This is xOR's encryption and decryption algorithm for textual information.
It's just a example and work study for myself.

Thank you **xOR** for your great tutorial!

All credits go to him, I just sinked it into code.

For details, in German, visit:
https://ngb.to/t/26584-Workshop-Hand-Verschlüsselung

## Usage

Make: `make`

Run encrypt on *input.txt*, output to *encrypted.txt*:
`./xor -e pepperandsalt -v cucumbersalat withoutTomato -iinput.txt -oencrypted.txt`


Run decrypt:
`./xor -d pepperandsalt -v cucumbersalat withoutTomato -iencrypted.txt -odecrypted.txt`


Keybase is *pepperandsalt* dice one key *cucumbersalat* and dice two password *withoutTomato*.
Please notice that everything becomes lowercased and punctuation and such become stripped.
