# Titan-Agent 开发环境盘点

> 2026-05-21 | 主机: hu-5950x-dual2080ti | 451G total

## 硬件设备

| 设备 | 接口 | 状态 |
|------|------|------|
| ESP32-C3 ABRobot OLED | /dev/ttyACM0 | 就绪 |
| RP2040 (Pico) | USB BOOTSEL | 就绪 |

## 本地大目录

| 路径 | 大小 | 可清理 |
|------|------|--------|
| ~/ai/models | 106G | 保留 |
| ~/conda | 29G | 保留 |
| ~/ai/vllm-qwen36 | 7.9G | 保留 |
| ~/esp/esp-idf | 3.4G | 需保留 |
| ~/Softwares/qemu-8.1.0 | 2.1G | 可存档 |
| ~/.espressif | 1.4G | 缓存 |
| ~/.hermes | 1.1G | sessions |

## LAN 基础设施 (192.168.2.120)

| 服务 | 端口 | 状态 |
|------|------|------|
| Gitea | :3002/:2222 | 运行 (DB索引需修) |
| Gerrit | :8081/:29418 | 运行 |
| Chromium | :3000-3001 | 运行 |
| SkillHub | :3876 | 停 |

## Gitea 仓库 (20个)

f133-apps(546M) f133-boot(44M) f133-drivers(496K) f133-firmware(46M)
f133-linux(206M) f133-notes(84M) f133-toolchains(1.1G)
taishanpi3-linux-sdk(3.9G) taishanpi3-notes(200K)
skills(416M) self-skills(1.4M) skillhub-lan(608K)
happy(110M) mcu-project(19M) ai-3d-printing-automation(1.7M)
call-pipeline(560K) bambu-lab-cri-*(324K) adb-contacts-export(140K)
nas-env-notes(160K)

## Gerrit 项目 (10个)

devshared: f133-apps f133-boot f133-drivers f133-linux f133-notes
f133-toolchains self-skills skillhub-lan
pilot-gerrit-sandbox

## GitHub (chatHulu, 17个)

dotfiles hermes-agents-memory rp2040-pico-playground
esp32c3-abrobot hermes-skills Fabric openhuman
qpc-experiments quant-journey llm-tracker
clawdbot dev-env-wizard clawdbot-*(5个)
