# 引数 config.yml
## data/sec [byte/msec]
## ロス率，遅延
## サブネット <= こちらで設定
## senderのIPアドレス　ポート <= こちらで設定
## receiverの数 <= こちらで設定
## reciever1 ipaddr port <= こちらで設定
## reciever2 ipaddr port <= こちらで設定
## .....

# docker-composeコード生成
from jinja2 import Template
from yaml import safe_load

with open("config.yml") as f:
    data = safe_load(f)

working_dir = ""
volumes = ""
ipaddr = ""

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
        tty: true
        command: bash -c \"bash tc.sh {{}}\"
    
    {% for i in [1, 2]  %}
    receiver{{i}}:
        image: sample
        working_dir: {{i}}
        volumes:
            - {{i}}
        networks:
            intra_net:
                ipv4_address: {{i}}
        cap_add:
            - NET_ADMIN
        tty: true
        command: bash -c \"bash tc.sh {{i}}\"
    {% endfor %}

networks:
    intra_net:
        ipam:
            driver: default
            confg:
                - subnet: {{sub}}
''')

print(template.render())