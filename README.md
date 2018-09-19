# activity-tracker
> An agent of personal activity monitoring system for Windows desktop.

`activity-tracker`는 포어 그라운드 윈도우상의 활동을 모니터링하여 `Fluentd` 서버로 전송, `Elasticsearch`등의 스토리지에 저장하고 가시화하는 시스템의 에이전트 프로그램입니다. `activity-tracker`가 수집하는 정보는 다음과 같습니다.

* filename: 포어 그라운드 윈도우의 파일명 ex) Diablo III.exe
* window_text: 포어 그라운드 윈도우의 타이틀 ex) 디아블로 III
* action_per_second: 초당 키보드나 마우스 입력의 횟수
* status: 현재 상태
  * 30초 이상 키보드나 마우스 입력이 없을 경우 idle
  * 이외의 경우는 active
* session_id: 지속적으로 status가 active 상태임을 알리 위한 session_id
  * idle 상태에서 active로 변환될 때 새로운 session_id가 발행됨
  * Windows의 session id와는 별개임
* user: 현재 로그인 상태의 윈도우즈 계정

## 컴파일 방법

`Visual Studio`를 이용해 `build` 디렉토리의 솔루션을 빌드합니다.

## 사용예

다음과 같이 `fluentd`의 호스트IP와 UDP 소스의 포트 번호를 지정하여 실행합니다. 전송된 활동 로그를 `Elasticsearch`등의 스토리지에 저장하는 설정에 관해선 `fluentd`의 문서등을 참고해주십시오.

```sh
activity-tracker.exe 192.168.11.5 20039
```

활동 로그를 `Elasticsearch`에 저장하고 `Kibana`를 사용해 가시화하면 다음과 같이 자신의 활동 내용을 가시화 해볼 수 있습니다.


![dashboard-example](images/dashboard.png)
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2FHatsuneMiku3939%2Factivity-tracker.svg?type=shield)](https://app.fossa.io/projects/git%2Bgithub.com%2FHatsuneMiku3939%2Factivity-tracker?ref=badge_shield)

## 풀 리퀘스트

자유롭게 풀 리퀘스트를 생성해주십시오.

### 솔루션 파일을 생성하는 방법

`src` 디렉토리에 파일을 추가하셨다면 솔루션 파일을 재생성할 필요가 있습니다. `build` 디렉토리의 솔루션 파일은 `Premake5`를 사용해 생성되었습니다. premake5 [https://premake.github.io/download.html](https://premake.github.io/download.html)을 내려받으신 후 `build` 디렉토리에서 다음 명령을 실행하시면 솔루션 파일을 재생성할 수 있습니다. 솔루션 파일의 재생성하면 `src` 디렉토리에 추가, 작제된 `cpp` 파일과 `h` 파일을 인식하여 솔루션 파일이 생성됩니다.

```sh
premake5 vs2013
```

## 외부 라이브러리
`activity-tracker`는 다음의 라이브러리와 툴들을 사용하고 있습니다. 훌륭한 라이브러리 및 툴을 만들어준 개발자들에게 감사드립니다.

* [jsoncpp](https://github.com/open-source-parsers/jsoncpp)
* [premake](https://premake.github.io/)

## 라이센스
MIT License


## License
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2FHatsuneMiku3939%2Factivity-tracker.svg?type=large)](https://app.fossa.io/projects/git%2Bgithub.com%2FHatsuneMiku3939%2Factivity-tracker?ref=badge_large)