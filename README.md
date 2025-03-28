# INTEGRAT-reading-the-mind-of-the-AI-robot
EXECUTIVE SUMMARY

The INTEGRAT "phoneme recognizer" executable runs on Windows, and the source code is also provided. The example data can be used to train an artificial intelligence robot to recognize a set of four phonemes. The user can experiment with a variety of AI parameters, and work those parameters backwards to understand the internal workings of the automated classifier, reading the mind of the robot so to speak. 

Here is a YouTube demonstration...https://youtu.be/ElCA72b_jMM

DETAILS

Reading the Robot Mind

It used to be that only human experts examined data and made decisions. Now Artificial Intelligence (AI) is enabling robotic decision making in an ever-widening variety of applications. As society allows this to happen, there is a greater likelihood that these robot decisions can affect people’s lives. It makes sense, therefore, to understand the capabilities and societal implications of AI robots.

Big data is a term used to describe both the opportunities and the problems associated with so much information now available for decision making. With the advent of the Internet of Things (IoT) the impact of this huge amount of data is only growing. Actionable decisions need to be distilled from big data and AI can only go so far based on linear extrapolation. Because of this, many non-linear deep learning algorithms are being developed. 

What exactly have these AI robots learned so deeply from all of this big data? 

This question is very reasonable for society to ask. It is not enough to train and create a great AI robot. Many researchers are realizing that before their systems can be deployed, they must be able to prove to human experts that the robots learned the right things from the right data. This is difficult because human expert decision makers are not necessarily the same people who are good at creating robot AI. These two teams must work together in a user friendly way.

If only we could read the robot mind.

Recently some old research of mine has been getting increased attention by engineers and scientists working on exactly the issues discussed above. This research yielded four US patents and a Windows app that gives a friendly user interface to allow examination of big data and inner workings of a trained AI decision maker. I never publically released the software, called INTEGRAT, because I sold rights to the patents to my employer at the time. Now that the patents have expired, I can share my work, as well as the software specifically designed to read the mind of the robot.

To make this research more easily accessible I am releasing the software for Windows, a short YouTube video explaining how to use the software, and the source code on GitHub. The software recognizes phonemes, basic elements of human speech. The feature extraction is speech specific, but the AI algorithms are reusable. Both neural network (back propagation and self-organizing maps) as well as simpler centroid based classifiers (unsupervised and supervised competitive learning) can be used for bio-medical signal analysis, stock market prediction, or anything in between. In all cases, human experts are the key to making the best AI decisions, and INTEGRAT is designed to help that human expert analyze that robot, as well as the data used to train, it in a user friendly way. 

Below is a list of the questions the research seeks to answer. Aside: As the Patent Liaison to The United States for the Swedish company Ericsson, I learned how to organize ideas into intellectual property terms, so for even more details, the reader is directed to the specific patent and independent claims listed below, all publically available at www.USPTO.gov.

US Patent 5809462 – 

Question: IS THE FEATURE EXTRACTION ALGORITHM GOOD AND CAN THE FEATURES BE USED AS A CODEC? 

All AI systems train on data from the real world gathered by sensors, but before the data can make it to the AI, it must be digitized and important features must be extracted (sometimes called data transformation). INTEGRAT allows a human expert to determine if the features extracted are suitable to train a robot to identify the correct decision.

Independent Claim 1 – Provide a friendly user interface to present the features to the eyes and ears of the human expert to see if they can make the correct decisions based on the transformation features alone.

Independent Claim 14 – Use that same user interface to let the human expert make sure that the data used to train one possible decision all seem similar to each other, and different from data which should yield different decisions.

Independent Claim 25 – Let the human expert see if the features contain enough information to be used as a codec. In other words, if we can transform the original data into a set of features, we should be able to create original data (somewhat distorted and simplified perhaps) from the features – with sufficient information for a human expert to use that to identify the correct decision. 

US Patent 5749066 – 

Question: IS THE TRAINING DATA GOOD AND CAN THE CLASSIFIER BE USED AS A CODEC?

AI robots can be finicky about the actual examples selected to be used for training. Giving too many of one kind of example can have a negative impact on the resulting decision maker, as can the introduction of misleading data examples (either accidentally or maliciously). Sometimes, a training set can be obviously bad, and other times, only after the AI is trained, can the human expert see that something has gone wrong.

Independent Claims 1 and 7 - Provide a friendly user interface to present all stages of creating the training set to the eyes and ears of the human expert to see if they can spot any problems noted above.

Independent Claim 3 – Let the human expert see if the AI can be used as a codec. In other words, if the AI can identify the correct decision, then we can work backwards through the AI layers to recreate an exemplary feature set. We can then create the original data (somewhat distorted and simplified perhaps) from those features – with sufficient information for a human expert to use that to identify the correct decision.

US Patent 5864803

Question: HOW CAN WE ASSIGN MORE THAN ONE CORRECT CLASSIFICATION WHEN EXPERTS CANNOT AGREE?

Because data sets can be very large and continuously streaming in from many sources, AI robot creators will chop up the data into segments that can be presented to identify a decision. Sometimes different human experts will see the same data segment and identify different decisions. Similarly, sometimes a human expert will examine one data segment and come up with two possible decisions, each of which are valid. INTEGRAT accounts for this situation in all stages of the creation and testing of the data and the AI robot.

Independent Claims 1 and 6 - Provide a friendly user interface to present data segments to the human expert and let them change the segmentation algorithm, or manually change a segment, if it could yield more than one decision.

Independent Claims 13, 17, and 20 – Provide a mechanism whereby multiple “correct” decisions can be assigned to one data segment, as well as AI training algorithms to support this ambiguity. Finally, allow the AI robot to come up with a single “best” decision when presented with ambiguous data, while also alerting users to “second best” decisions, and so on.

US Patent 5867816

Question: WILL THE ROBOT FUNCTION JUST AS WELL WHEN THE DATA SEGMENTATION, FEATURE EXTRACTION, AND DECISION IDENTIFIER ARE FULLY AUTOMATED?

The AI Robot is not useful if it requires a human expert to accompany the device when it is deployed in the field for day to day use. The robot must work on its own. Nevertheless, a quality assurance mechanism is required that makes the robot demonstrate it is working in “fully automatic” mode while a human expert grades it on the decisions the AI has made.

Independent Claims 1, 11, and 29 – The human expert examines and modifies the segmentation, features, and decision identifications to improve automated functioning.


I hope these methods will help to improve the understanding by individuals and society of the capabilities and societal implications of conventional and emerging technologies, including intelligent systems.

Best Regards, Paul Alton Nussbaum, Ph.D. 

