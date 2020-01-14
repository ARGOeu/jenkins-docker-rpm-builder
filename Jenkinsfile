pipeline {
    agent { docker { image 'ubuntu' } }
    stages {
        stage('build') {
            steps {
                sh 'mvn --version'
            }
        }
    }
}