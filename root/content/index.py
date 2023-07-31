import random

# List of jokes
jokes = [
    "Why did the programmer quit his job? He didn't get arrays.",
    "Why do programmers prefer dark mode? Less white space.",
    "Why do programmers hate nature? It has too many bugs.",
    "Why did the programmer go broke? He used up all his cache.",
    "Why did the programmer get kicked out of school? He couldn't handle the recursion.",
    "Why did the programmer get fired from the fruit factory? He couldn't concentrate.",
    "Why did the computer go to school? To get smarter with its 'bits' of knowledge!",
    "Why did the smartphone enroll in university? To earn a degree in 'app-lication' development!",
    "Why did the social media influencer fail math class? They couldn't count their followers!",
    "What did the teacher say to the student who was always on social media in class? 'Stop posting and start 'learning'!'",
    "Why did the grammar teacher refuse to use social media? They didn't want to deal with too many 'missed takes' and 'no edits'!",
    "Why did the university student break up with their laptop? It was always too 'distant' with its Wi-Fi connection!",
    "What's a social media platform's favorite subject in school? #History, of course!",
    "Why was the Twitter bird the most popular student in school? It knew how to 'tweet' everyone kindly!",
    "Why did the student get kicked out of the social media club? They couldn't stop 'posting' bad content!",
    "What did the university student say when asked why they were always on social media? 'It's just my way of 'connecting' with the world!'",
    "Why did the computer get a job at the university? It passed the 'byte-sized' interview!",
    "What's a social media ghost's favorite subject in school? Haunting-tology!",
    "Why did the math book look sad on social media? It couldn't find its 'X' in all those selfies!",
    "Why did the smartphone attend art school? To perfect its 'filter' techniques!",
    "Why did the laptop go to therapy? It had too many 'attachment issues' with files!",
    "Why was the teacher good at social media? They knew how to 'teach' people new trends!",
    "Why was the university professor always active on social media? They loved 'sharing' their knowledge!",
    "What did the student say when their phone died during class? 'I've lost the 'connection' to the outside world!'",
    "Why did the social media manager do well in geography class? They were great at 'pinpointing' locations!",
    "What's a university student's favorite social media challenge? The 'procrastination' challenge!",
    "Why did the Facebook post go to school? To 'share' its experiences with everyone!",
    "Why did the YouTube video become a teacher? It had the most 'views' on educational content!",
    "What's a social media platform's favorite type of music? 'Snapchat' music!",
    "Why did the smartphone start a band in university? It wanted to become a 'multi-talented' device!",
    "What did the social media influencer say about graduating from university? 'Finally, I've completed my 'studies' in style!'"
]

def get_random_joke(jokes_list):
    return random.choice(jokes_list)

if __name__ == "__main__":
	print("<html><body><h1>" + get_random_joke(jokes) + "</h1></body></html>")
