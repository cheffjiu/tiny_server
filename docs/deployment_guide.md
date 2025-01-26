# 项目部署指南

## 1. 环境准备

### 系统要求
- 操作系统：Linux（推荐Ubuntu 20.04或CentOS 7）
- 内存：建议2GB及以上
- 磁盘空间：10GB以上

### 依赖安装
```bash
# Ubuntu系统
sudo apt update
sudo apt install -y build-essential cmake mysql-server mysql-client libmysqlclient-dev git

# CentOS系统
sudo yum update
sudo yum groupinstall "Development Tools"
sudo yum install -y cmake mysql-server mysql-devel git
```

## 2. 代码部署

### 获取代码
```bash
# 克隆代码到服务器
git clone [你的代码仓库地址]
cd tiny_server
```

### 编译项目
```bash
# 创建并进入构建目录
mkdir build && cd build

# 配置并编译
cmake ..
make -j$(nproc)
```

## 3. 数据库配置

### 启动MySQL服务
```bash
# Ubuntu系统
sudo systemctl start mysql
sudo systemctl enable mysql

# CentOS系统
sudo systemctl start mysqld
sudo systemctl enable mysqld
```

### 创建数据库和用户
```sql
-- 登录MySQL
mysql -u root -p

-- 创建数据库
CREATE DATABASE tiny_server;

-- 创建用户并授权
CREATE USER 'webserver'@'localhost' IDENTIFIED BY 'your_password';
GRANT ALL PRIVILEGES ON tiny_server.* TO 'webserver'@'localhost';
FLUSH PRIVILEGES;
```

## 4. 服务器配置

### 防火墙设置
```bash
# Ubuntu系统
sudo ufw allow 80/tcp    # HTTP端口
sudo ufw allow 3306/tcp  # MySQL端口（如需远程访问）

# CentOS系统
sudo firewall-cmd --permanent --add-port=80/tcp
sudo firewall-cmd --permanent --add-port=3306/tcp
sudo firewall-cmd --reload
```

### 配置服务
1. 修改配置文件（如有需要）：
   - 数据库连接信息
   - 服务器监听端口
   - 日志路径等

## 5. 启动服务

### 运行Web服务器
```bash
# 进入编译目录
cd build

# 启动服务器
./tiny_server
```

### 设置开机自启（可选）
```bash
# 创建系统服务文件
sudo vim /etc/systemd/system/tiny_server.service

[Unit]
Description=Tiny Web Server
After=network.target mysql.service

[Service]
Type=simple
ExecStart=/path/to/tiny_server/build/tiny_server
WorkingDirectory=/path/to/tiny_server/build
Restart=always
User=www-data

[Install]
WantedBy=multi-user.target

# 启用服务
sudo systemctl enable tiny_server
sudo systemctl start tiny_server
```

## 6. 验证部署

1. 检查服务状态：
```bash
sudo systemctl status tiny_server  # 如果配置了系统服务
# 或者
ps aux | grep tiny_server
```

2. 测试Web访问：
   - 通过服务器IP访问：`http://your_server_ip`
   - 检查服务器日志文件

## 7. 维护建议

1. 日志管理
   - 定期检查日志文件
   - 配置日志轮转

2. 数据库维护
   - 定期备份数据库
   - 监控数据库性能

3. 性能监控
   - 监控CPU和内存使用
   - 监控网络流量

4. 安全建议
   - 及时更新系统和依赖包
   - 使用SSL/TLS加密（如需要）
   - 定期检查安全日志

## 故障排除

1. 服务无法启动
   - 检查日志文件
   - 验证配置文件
   - 确认端口是否被占用

2. 数据库连接失败
   - 检查MySQL服务状态
   - 验证数据库用户权限
   - 确认连接配置正确

3. 访问超时
   - 检查防火墙配置
   - 验证网络连接
   - 检查服务器负载