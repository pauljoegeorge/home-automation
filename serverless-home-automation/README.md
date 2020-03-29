# Serverless Alexa Skill

### Python version
- Python3.6

### AWS Lambda 
  - Create a new lambda function 
  - Upload the zip file
  - Choose the python version to 3.6
  - Add `trigger` Alexa
      - Paste Alexa Skill ID from Alexa developer console

  - Add Lambda ARN to Alexa skill
 
 
 ### How to create Zip file?
> Recommended to create it in Amazon Linux env
- create venv for python3.6
- zip packages
    - `$ pip install <package-name> --target . `
    - above command will install package to current directory
    - similary get all pakcages and `lambda_function.py` in one file
    - `$ zip -r9 ../lambda_function.zip .`  will zip all files in current directory and save it to ../directory with a file named lambda_function.zip



### Sorry for the horrible documentation :| 


