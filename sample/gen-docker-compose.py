# 引数 config.yml
## ロス率，遅延
## サブネット <= こちらで設定
## senderのIPアドレス　ポート <= こちらで設定
## receiverの数 <= こちらで設定
## reciever1 ipaddr port <= こちらで設定
## reciever2 ipaddr port <= こちらで設定
## .....
## command ./sender
## command ./receiver

# docker-composeコード生成
from jinja2 import Template
from yaml import safe_load

with open("config.yml") as f:
    data = safe_load(f)

mnt_dir = "/usr/src/sample/"
working_dir = "" # mnt_dir/241116-000000-0%-0ms
volumes = ".:/usr/src/sample"
ipaddr = "192.168.0.0/24"

print(data)

template = Template('''
version: \'3\'
services:
    sender:
        image: sample
        working_dir: {{working_dir}}
        volumes:
            - {{volumes}}
        networks:
            intra_net:
                ipv4_address: {{ipaddr}}
        cap_add:
            - NET_ADMIN
        depends_on:
            - {{receiver}}

networks:
    intra_net:
        ipam:
            driver: default
            confg:
                - subnet: {{ipaddr}}
''')

print(template.render(working_dir=working_dir, volumes=volumes, ipaddr=ipaddr,receiver='receiver'))