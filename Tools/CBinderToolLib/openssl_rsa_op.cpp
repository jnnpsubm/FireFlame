
#include <stdio.h>   // rsa_op.cpp
#include <string>
#include <iostream>
#include "openssl_rsa_op.h"

rsa_op::rsa_op()
{
    _pub_key = NULL;
    _pri_key = NULL;

    _pub_expd = NULL;
    _pri_expd = NULL;
    _module = NULL;
    _pub_expd_len = 0;
    _pri_expd_len = 0;
    _module_len = 0;
}

rsa_op::~rsa_op()
{
    close_key();
    free_res();
}

// 初始化参数
int rsa_op::set_params(const unsigned char *pub_expd, int pub_expd_len,
    const unsigned char *pri_expd, int pri_expd_len,
    const unsigned char *module, int module_len)
{
    if (pub_expd)
    {

        _pub_expd_len = pub_expd_len;
        _pub_expd = new unsigned char[pub_expd_len];
        if (!_pub_expd)
        {
            free_res();
            return -1;
        }

        memcpy(_pub_expd, pub_expd, _pub_expd_len);
    }

    if (pri_expd)
    {
        _pri_expd_len = pri_expd_len;
        _pri_expd = new unsigned char[pri_expd_len];
        if (!_pri_expd)
        {
            free_res();
            return -1;
        }

        memcpy(_pri_expd, pri_expd, pri_expd_len);
    }

    if (module)
    {
        _module_len = module_len;
        _module = new unsigned char[module_len];
        if (!_module)
        {
            free_res();
            return -1;
        }

        memcpy(_module, module, module_len);
    }

    return 0;
}

// 在一个key中同时打开公钥和私钥，该key既可用作公钥函数，也可用作私钥函数
int rsa_op::open_prikey_pubkey()
{
    //构建RSA数据结构
    _pri_key = RSA_new();
    _pri_key->e = BN_bin2bn(_pub_expd, _pub_expd_len, _pri_key->e);
    _pri_key->d = BN_bin2bn(_pri_expd, _pri_expd_len, _pri_key->d);
    _pri_key->n = BN_bin2bn(_module, _module_len, _pri_key->n);

    RSA_print_fp(stdout, _pri_key, 0);

    return 0;
}

// 打开私钥
int rsa_op::open_prikey()
{
    //构建RSA数据结构
    _pri_key = RSA_new();
    //_pri_key->e = BN_bin2bn(_pub_expd, _pub_expd_len, _pri_key->e);
    _pri_key->d = BN_bin2bn(_pri_expd, _pri_expd_len, _pri_key->d);
    _pri_key->n = BN_bin2bn(_module, _module_len, _pri_key->n);

    return 0;
}
// 打开公钥
int rsa_op::open_pubkey()
{
    //构建RSA数据结构
    _pub_key = RSA_new();
    _pub_key->e = BN_bin2bn(_pub_expd, _pub_expd_len, _pub_key->e);
    //_pub_key->d = BN_bin2bn(_pri_expd, _pri_expd_len, _pub_key->d);
    _pub_key->n = BN_bin2bn(_module, _module_len, _pub_key->n);

    RSA_print_fp(stdout, _pub_key, 0);

    return 0;
}

int rsa_op::open_pubkey_from_string(const std::string& key)
{
    BIO *bio = NULL;
    if ((bio = BIO_new_mem_buf(key.data(), (int)key.size())) == NULL)       //从字符串读取RSA公钥
    {
        std::cout << "BIO_new_mem_buf failed!" << std::endl;
    }

    _pub_key = PEM_read_bio_RSAPublicKey(bio, NULL, NULL, NULL);
    if (!_pub_key)
    {
        ERR_load_crypto_strings();
        char errBuf[512];
        ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
        std::cout << "load public key failed[" << errBuf << "]" << std::endl;
        BIO_free_all(bio);
    }
    return 0;
}

// 私钥加密函数
int rsa_op::prikey_encrypt(const unsigned char *in, int in_len,
    unsigned char **out, int &out_len)
{
    out_len = RSA_size(_pri_key);
    *out = (unsigned char *)malloc(out_len);
    if (NULL == *out)
    {

        printf("prikey_encrypt:malloc error!\n");
        return -1;
    }
    memset((void *)*out, 0, out_len);

    printf("prikey_encrypt:Begin RSA_private_encrypt ...\n");
    int ret = RSA_private_encrypt(in_len, in, *out, _pri_key, RSA_PKCS1_PADDING);
    //RSA_public_decrypt(flen, encData, decData, r,  RSA_NO_PADDING);

    return ret;
}
// 公钥解密函数，返回解密后的数据长度
int rsa_op::pubkey_decrypt(const unsigned char *in, int in_len,
    unsigned char **out, int &out_len)
{
    out_len = RSA_size(_pub_key);
    *out = (unsigned char *)malloc(out_len);
    if (NULL == *out)
    {
        printf("pubkey_decrypt:malloc error!\n");
        return -1;
    }
    memset((void *)*out, 0, out_len);

    //printf("pubkey_decrypt:Begin RSA_public_decrypt ...\n");
    return RSA_public_decrypt(in_len, in, *out, _pub_key, RSA_NO_PADDING);
}
// 公钥加密函数
int rsa_op::pubkey_encrypt(const unsigned char *in, int in_len,
    unsigned char **out, int &out_len)
{
    out_len = RSA_size(_pub_key);
    *out = (unsigned char *)malloc(out_len);
    if (NULL == *out)
    {
        printf("pubkey_encrypt:malloc error!\n");
        return -1;
    }
    memset((void *)*out, 0, out_len);

    printf("pubkey_encrypt:Begin RSA_public_encrypt ...\n");
    int ret = RSA_public_encrypt(in_len, in, *out, _pub_key, RSA_PKCS1_PADDING/*RSA_NO_PADDING*/);


    return ret;
}

// 私钥解密函数，返回解密后的长度
int rsa_op::prikey_decrypt(const unsigned char *in, int in_len,
    unsigned char **out, int &out_len)
{
    out_len = RSA_size(_pri_key);
    *out = (unsigned char *)malloc(out_len);
    if (NULL == *out)
    {
        printf("prikey_decrypt:malloc error!\n");
        return -1;
    }
    memset((void *)*out, 0, out_len);

    printf("prikey_decrypt:Begin RSA_private_decrypt ...\n");
    int ret = RSA_private_decrypt(in_len, in, *out, _pri_key, RSA_PKCS1_PADDING);

    return ret;
}

// 释放分配的内存资源
void rsa_op::free_res()
{
    if (_pub_expd)
    {
        delete[]_pub_expd;
        _pub_expd = NULL;
    }

    if (_pri_expd)
    {
        delete[]_pri_expd;
        _pri_expd = NULL;
    }
    if (_module)
    {
        delete[]_module;
        _module = NULL;
    }
}

// 释放公钥和私钥结构资源
int rsa_op::close_key()
{
    if (_pub_key)
    {
        RSA_free(_pub_key);
        _pub_key = NULL;
    }

    if (_pri_key)
    {
        RSA_free(_pri_key);
        _pri_key = NULL;
    }

    return 0;
}

