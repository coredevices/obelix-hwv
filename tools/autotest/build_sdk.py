# -*- coding: utf-8 -*-
from __future__ import print_function
import argparse
import datetime
import os
import subprocess
import sys
import yaml
import shutil
import codecs
import tempfile
import platform


class SDKBuilder(object):
    def __init__(self, config_file, compiler="keil"):
        self.config = self._load_config(config_file)
        self.compiler = compiler
        self.root_dir = os.getcwd()
        self.failed_projects = []
        self.build_log_dir = os.path.join(self.root_dir, self.config['common']['log_dir'])
        if not os.path.exists(self.build_log_dir):
            os.makedirs(self.build_log_dir)
        self.is_windows = platform.system() == 'Windows'

    def _load_config(self, config_file):
        with codecs.open(config_file, 'r', encoding='utf-8') as f:
            return yaml.safe_load(f)

    def _create_build_script(self, commands, work_dir=None, log_file=None):
        """创建构建脚本，根据不同的操作系统创建.bat或.sh文件"""
        if self.is_windows:
            return self._create_windows_script(commands, work_dir, log_file)
        else:
            return self._create_unix_script(commands, work_dir, log_file)

    def _create_windows_script(self, commands, work_dir=None, log_file=None):
        """创建Windows构建脚本(.bat)"""
        fd, path = tempfile.mkstemp(suffix='.bat')
        try:
            with os.fdopen(fd, 'w') as f:
                f.write('@echo off\n')
                if log_file:
                    log_dir = os.path.dirname(log_file)
                    if log_dir:
                        f.write('if not exist "{0}" mkdir "{0}"\n'.format(log_dir))

                f.write('call "{0}" {1}\n'.format(os.path.join(self.root_dir, "set_env.bat"), self.compiler))

                if work_dir:
                    f.write('cd /d "{0}"\n'.format(work_dir))

                for cmd in commands:
                    if log_file:
                        f.write('echo [%date% %time%] Executing: {0} >> "{1}"\n'.format(cmd, log_file))
                        f.write('{0} >> "{1}" 2>&1\n'.format(cmd, log_file))
                        f.write('set BUILD_ERROR=%ERRORLEVEL%\n')
                        f.write('if %BUILD_ERROR% neq 0 (\n')
                        f.write(
                            '    echo [%date% %time%] Command failed with error code %BUILD_ERROR% >> "{0}"\n'.format(
                                log_file))
                        f.write('    exit /b %BUILD_ERROR%\n')
                        f.write(')\n')
                    else:
                        f.write('{0}\n'.format(cmd))
            return path
        except Exception as e:
            os.remove(path)
            raise e

    def _create_unix_script(self, commands, work_dir=None, log_file=None):
        """创建Unix构建脚本(.sh)，适用于Linux和macOS"""
        fd, path = tempfile.mkstemp(suffix='.sh')
        try:
            with os.fdopen(fd, 'w') as f:
                f.write('#!/bin/bash\n')
                
                if log_file:
                    log_dir = os.path.dirname(log_file)
                    if log_dir:
                        f.write('mkdir -p "{0}"\n'.format(log_dir))

                # 使用export.sh脚本设置环境
                f.write('source "{0}" {1}\n'.format(os.path.join(self.root_dir, "export.sh"), self.compiler))

                if work_dir:
                    f.write('cd "{0}" || exit 1\n'.format(work_dir))

                for cmd in commands:
                    if log_file:
                        f.write('echo "[$(date)] Executing: {0}" >> "{1}"\n'.format(cmd, log_file))
                        f.write('{0} >> "{1}" 2>&1\n'.format(cmd, log_file))
                        f.write('BUILD_ERROR=$?\n')
                        f.write('if [ $BUILD_ERROR -ne 0 ]; then\n')
                        f.write('    echo "[$(date)] Command failed with error code $BUILD_ERROR" >> "{0}"\n'.format(log_file))
                        f.write('    exit $BUILD_ERROR\n')
                        f.write('fi\n')
                    else:
                        f.write('{0}\n'.format(cmd))
            # 确保脚本可执行
            os.chmod(path, 0o755)
            return path
        except Exception as e:
            os.remove(path)
            raise e

    def _run_commands(self, commands, work_dir=None, log_file=None):
        batch_file = self._create_build_script(commands, work_dir, log_file)
        try:
            process = subprocess.Popen(
                'cmd.exe /c "{0}"'.format(batch_file) if self.is_windows else 'bash "{0}"'.format(batch_file),
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                shell=True
            )
            stdout, stderr = process.communicate()

            if log_file and os.path.exists(log_file):
                with open(log_file, 'a') as f:
                    f.write('\n=== Additional Build Output ===\n')
                    if stdout:
                        f.write('=== STDOUT ===\n{0}\n'.format(stdout))
                    if stderr:
                        f.write('=== STDERR ===\n{0}\n'.format(stderr))

            return process.returncode, stdout, stderr
        finally:
            try:
                os.remove(batch_file)
            except:
                pass

    def build_project(self, project_path, board=None, cpu=None, is_common=False):
        try:
            if is_common:
                cmd = 'scons --board={0} -j8'.format(board)
            else:
                cmd = 'scons -j8'

            if cpu:
                project_path = os.path.join(project_path, cpu)

            # timestamp = datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
            project_path_normalized = project_path.replace('\\', '/')

            project_name = project_path_normalized.replace('/', '_').replace(':', '')
            if board:
                log_file = os.path.join(self.build_log_dir, '{0}_{1}.log'.format(
                    project_name, board))
            else:
                log_file = os.path.join(self.build_log_dir, '{0}.log'.format(
                    project_name))

            print("\nBuilding {0} {1}".format(project_path, "for " + board if board else ""))
            print("Build log will be saved to: {0}".format(project_name))

            returncode, stdout, stderr = self._run_commands(
                [cmd],
                work_dir=os.path.abspath(project_path),
                log_file=log_file
            )

            if returncode != 0:
                self.failed_projects.append((project_path, board if board else "", log_file))
                print("xxxxx Build failed for {0} {1} ".format(project_path, "(" + board + ")" if board else ""))
                print("xxxxx Error details can be found in: {0} ".format(project_name))
                return False

            print("Successfully built {0} {1}".format(project_path, "for " + board if board else ""))
            return True

        except Exception as e:
            print("Error building {0}: {1}".format(project_path, str(e)))
            self.failed_projects.append(
                (project_path, board if board else "", log_file if 'log_file' in locals() else None))
            return False

    def build_all(self):
        print("\n--------- Building normal projects ---------")
        if "projects" in self.config:
            for project in self.config['projects']:
                for prj_path in project['path']:
                    self.build_project(prj_path)

        print("\n--------- Building common projects ---------")
        if "common_projects" in self.config:
            for common_project in self.config['common_projects']:
                for board in common_project['boards']:
                    self.build_project(
                        common_project['path'],
                        board=board,
                        is_common=True
                )

    def generate_report(self):
        log_dir = os.path.join(self.config['common']['report_dir'])
        if not os.path.exists(log_dir):
            os.makedirs(log_dir)
        report_path = os.path.join(
            self.config['common']['report_dir'],
            'build_report.txt'
        )
        with codecs.open(report_path, 'w', encoding='utf-8') as f:
            f.write(u"SDK Build Report\n")
            f.write(u"=" * 50 + u"\n")
            f.write(u"Build Time: {0}\n\n".format(datetime.datetime.now()))
            f.write(u"=" * 50 + u"\n")
            if self.failed_projects:
                f.write(u"Failed Projects:\n")
                for project, board, log_file in self.failed_projects:
                    f.write(u"- {0} {1}\n".format(project, "(" + board + ")" if board else ""))
                    # if log_file:
                    #     f.write(u"  Log file: {0}\n".format(log_file))
                f.write(u"=" * 50 + u"\n\n")
            else:
                f.write(u"All projects built successfully!\n")

            # f.write(u"\nBuild logs directory: {0}\n".format(self.build_log_dir))

            f.write(u"All build projects:\n\n")
            f.write(u"--------- normal projects ---------\n")
            if "projects" in self.config:
                for project in self.config['projects']:
                    f.write(u"\n{0}\n".format(project['group']))
                    for prj_path in project['path']:
                        f.write(u"- {0}\n".format(prj_path))
            f.write(u"\n---------  common projects ---------\n")
            if "common_projects" in self.config:
                for common_project in self.config['common_projects']:
                    f.write(u"\n{0}\n".format(common_project['path']))
                    for board in common_project['boards']:
                        f.write(u"- {0}\n".format(board))
            f.write(u"\n")
            f.write(u"=" * 50 + u"\n")
        print("Build report generated at {0}".format(report_path))

    def backup_failed_logs(self):
        if not self.failed_projects:
            return

        backup_dir = os.path.join(self.root_dir, self.config['common']['report_dir'])
        if not os.path.exists(backup_dir):
            os.makedirs(backup_dir)

        for project, board, log_file in self.failed_projects:
            if log_file and os.path.exists(log_file):
                backup_file = os.path.join(backup_dir, os.path.basename(log_file))
                shutil.copy2(log_file, backup_file)


def main():
    parser = argparse.ArgumentParser(description='SDK Builder')
    parser.add_argument('-c', '--config', default='build_config.yaml',
                        help='Path to the build configuration file')
    parser.add_argument('-p', '--compiler', default='keil',
                        help='Compiler')                        
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='Enable verbose output')
    args = parser.parse_args()

    try:
        builder = SDKBuilder(args.config, args.compiler)
        builder.build_all()
        builder.generate_report()
        builder.backup_failed_logs()

        if builder.failed_projects:
            print("\n--------- Build completed with failures. See report for details. ---------")
            sys.exit(1)
        else:
            print("\n--------- Build completed successfully! ---------")
            sys.exit(0)

    except Exception as e:
        print("Error during build process: {0}".format(str(e)))
        sys.exit(1)


if __name__ == '__main__':
    main()
