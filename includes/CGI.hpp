#ifndef CGI_HPP
#define CGI_HPP

class CGI
{
public:
    CGI();
    ~CGI();
    void Execute(); //TODO: 子プロセスのなかでexecveを実行
private:
    char *argv[]; // execveで使用。newして作成予定なので、コンストラクタとかで作ってデストラクタでdeleteでいいかも
    char *envp[]; // *argv[]と同様
};

#endif /* CGI_HPP */
