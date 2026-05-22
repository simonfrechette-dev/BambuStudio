// Qt6 stub for FileTransferUtils.cpp
#include "FileTransferUtils.hpp"

namespace Slic3r {

FileTransferModule::FileTransferModule(ModuleHandle /*networking_module*/, int /*required_abi_version*/) {}

FileTransferTunnel::FileTransferTunnel(FileTransferModule& m, const std::string&) {}
void FileTransferTunnel::on_connection(std::function<void(bool, int, std::string)>) {}
void FileTransferTunnel::shutdown() {}
void FileTransferTunnel::start_connect() {}

FileTransferJob::FileTransferJob(FileTransferModule& m, const std::string&) {}
void FileTransferJob::on_result(std::function<void(int, int, std::string, std::vector<std::byte>)>) {}
void FileTransferJob::start_on(FileTransferTunnel&) {}

} // namespace Slic3r
