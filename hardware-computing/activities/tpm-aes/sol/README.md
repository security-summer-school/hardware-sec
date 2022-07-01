# TPM AES Decryption Challenge Solution

We are provided with an encrypted file, `secret.enc`, and with the public and the private parts of the AES key.

First, we need to create a primary object, with the following command:

```tpm2_createprimary -c primary.ctx```

Then, we can load the key, using `tpm2_load`:

```tpm2_load -C primary.ctx -u key.pub -r key.priv -c key.ctx```

Finally, we can decrypt the file:

```tpm2_encryptdecrypt -d -c key.ctx -o secret.dec secret.enc```
