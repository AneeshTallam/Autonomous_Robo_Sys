from twilio.rest import Client

account_sid = 'AC914e6c75a0103bc674fe1044925703a0'
auth_token = '3637442b4ce5d1a35da247fc5071f624'
client = Client(account_sid, auth_token)

call = client.calls.create(
    to='+4915753336416',
    from_='+15714895466',
    url='http://demo.twilio.com/docs/voice.xml'  # TwiML instructions
)

print("Call initiated:", call.sid)