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
import datetime
import sys

args = sys.argv

# YAML形式の設定ファイルを開く
with open(args[1]) as f:
    data = safe_load(f)

# 現在時刻の取得
dt_now = datetime.datetime.now()
dt_str = dt_now.strftime('%y%m%d-%H%M%S')

# 設定値の計算
mnt_dir = "/usr/src/sample/"
working_dir = '{}{}-{}%-{}ms'.format(mnt_dir, dt_str, data['network']['loss_rate'], data['network']['delay']) # mnt_dir/20241116-000000-0%-0ms
volumes = ".:{}".format(mnt_dir)
subnet = "192.168.0.0/24"
port = 10000

sender = {'name': 'sender', 'ipaddr': subnet.replace('0/24', str(10 + 1)), 'port': port}
receivers = []
for i in range(data['application']['replicas']):
    receivers.insert(i, {})
    receivers[i]['name'] = 'receiver{}'.format(i)
    receivers[i]['ipaddr'] = subnet.replace("0/24", str(20 + i))
    receivers[i]['port'] = port

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
                ipv4_address: {{sender['ipaddr']}}
        cap_add:
            - NET_ADMIN
        depends_on:
            {%- for receiver in receivers %}
            - {{receiver['name']}}
            {%- endfor %}
        tty: true

    {%- for receiver in receivers %}
    {{receiver['name']}}:
        image: sample
        working_dir: {{working_dir}}
        volumes:
            - {{volumes}}
        networks:
            intra_net:
                ipv4_address: {{receiver['ipaddr']}}
        cap_add:
            - NET_ADMIN
        tty: true
    {% endfor %}

networks:
    intra_net:
        ipam:
            driver: default
            config:
                - subnet: {{subnet}}
''')

output = template.render(working_dir=working_dir, volumes=volumes, sender=sender, receivers=receivers, subnet=subnet)
print(output)