
#!/usr/bin/env python
# Implementation of collaborative filtering recommendation engine


from recommendation_data import dataset
from math import sqrt

def similarity_score(person1,person2):
	
	# Returns ratio Euclidean distance score of person1 and person2 

	both_viewed = {}		# To get both rated items by person1 and person2

	for item in dataset[person1]:
		if item in dataset[person2]:
			both_viewed[item] = 1

		# Conditions to check they both have an common rating items	
		if len(both_viewed) == 0:
			return 0

		# Finding Euclidean distance 
		sum_of_eclidean_distance = []	

		for item in dataset[person1]:
			if item in dataset[person2]:
				sum_of_eclidean_distance.append(pow(dataset[person1][item] - dataset[person2][item],2))
		sum_of_eclidean_distance = sum(sum_of_eclidean_distance)

		return 1/(1+sqrt(sum_of_eclidean_distance))



def pearson_correlation(person1,person2):

	# To get both rated items
	both_rated = {}
	for item in dataset[person1]:
		if item in dataset[person2]:
			both_rated[item] = 1

	number_of_ratings = len(both_rated)		
	
	# Checking for number of ratings in common
	if number_of_ratings == 0:
		return 0

	# Add up all the preferences of each user
	person1_preferences_sum = sum([dataset[person1][item] for item in both_rated])
	person2_preferences_sum = sum([dataset[person2][item] for item in both_rated])

	# Sum up the squares of preferences of each user
	person1_square_preferences_sum = sum([pow(dataset[person1][item],2) for item in both_rated])
	person2_square_preferences_sum = sum([pow(dataset[person2][item],2) for item in both_rated])

	# Sum up the product value of both preferences for each item
	product_sum_of_both_users = sum([dataset[person1][item] * dataset[person2][item] for item in both_rated])

	# Calculate the pearson score
	numerator_value = product_sum_of_both_users - (person1_preferences_sum*person2_preferences_sum/number_of_ratings)
	denominator_value = sqrt((person1_square_preferences_sum - pow(person1_preferences_sum,2)/number_of_ratings) * (person2_square_preferences_sum -pow(person2_preferences_sum,2)/number_of_ratings))
	if denominator_value == 0:
		return 0
	else:
		r = numerator_value/denominator_value
		return r 

def most_similar_users(person,number_of_users):
	# returns the number_of_users (similar persons) for a given specific person.
	scores = [(pearson_correlation(person,other_person),other_person) for other_person in dataset if  other_person != person ]
	
	# Sort the similar persons so that highest scores person will appear at the first
	scores.sort()
	scores.reverse()
	return scores[0:number_of_users]

def user_recommendations(person):

	# Gets recommendations for a person by using a weighted average of every other user's rankings
	totals = {}
	simSums = {}
	rankings_list =[]
	for other in dataset:
		# don't compare me to myself
		if other == person:
			continue
		sim = pearson_correlation(person,other)
		#sim = similarity_score(person,other)
		print("Indice di similarità tra",person,"and", other,">>",round(sim,2))

		# ignore scores of zero or lower
		if sim <=0: 
			continue
		for item in dataset[other]:

			# only score movies i haven't seen yet
			if item not in dataset[person] or dataset[person][item] == 0:

			# Similrity * score
				totals.setdefault(item,0)
				totals[item] += dataset[other][item]* sim
				print(f'\t {item}:{dataset[other][item]} * {round(sim,2)} = {round((dataset[other][item]* sim),2)}')
								
				# sum of similarities
				simSums.setdefault(item,0)
				simSums[item]+= sim

		# Create the normalized list

	rankings = [(total/simSums[item],item) for item,total in totals.items()]
	rankings.sort()
	rankings.reverse()
	# returns the recommended items
	recommendataions_list = [(recommend_item,round(score,2)) for score,recommend_item in rankings]
	return recommendataions_list

def user_recommendations3(person):
	# Gets recommendations for a person by using a weighted average of every other user's rankings
	totals = {}
	simSums = {}
	rankings_list =[]
	for other in dataset:
		# don't compare me to myself
		if other == person:
			continue
		#sim = pearson_correlation(person,other)
		sim = similarity_score(person,other)
		print("Indice di similarità tra",person,"and", other,">>",round(sim,2))

		# ignore scores of zero or lower
		if sim <=0: 
			continue
		for item in dataset[other]:

			# only score movies i haven't seen yet
			if item not in dataset[person] or dataset[person][item] == 0:

			# Similrity * score
				totals.setdefault(item,0)
				totals[item] += dataset[other][item]* sim
				print(f'\t {item}:{dataset[other][item]} * {round(sim,2)} = {round((dataset[other][item]* sim),2)}')
								
				# sum of similarities
				simSums.setdefault(item,0)
				simSums[item]+= sim

		# Create the normalized list

	rankings = [(total/simSums[item],item) for item,total in totals.items()]
	rankings.sort()
	rankings.reverse()
	# returns the recommended items
	recommendataions_list = [(recommend_item,round(score,2)) for score,recommend_item in rankings]
	return recommendataions_list

def calculateAverage(person):
	sum=0
	for item in dataset[person]:
			sum+=dataset[person][item]
	return  sum / len(dataset[other])



# def user_recommendations2(person):
# 	# Gets recommendations for a person by using a weighted average of every other user's rankings
# 	totals = {}
# 	rankings_list =[]
# 	simSums=0
# 	simSumProd=0
# 	for other in dataset:
# 		average = calculateAverage(other)
# 		# don't compare me to myself
# 		if other == person:
# 			continue
# 		#sim = pearson_correlation(person,other)
# 		sim = similarity_score(person,other)
# 		#print(person,"and", other,">>>>>>>",round(sim,2))

# 		# ignore scores of zero or lower
# 		if sim <=0: 
# 			continue
# 		for item in dataset[other]:

# 			# only score movies i haven't seen yet
# 			if item not in dataset[person] or dataset[person][item] == 0:

# 			# Similrity * score
# 				totals.setdefault(item,0)
# 				totals[item] += dataset[other][item]* sim
# 				########
# 				simSums+= sim 
# 				x = dataset[other][item] - average
# 				simSumProd+=sim*x
# 		rankings_list.append(calculateAverage(person)+ (simSumProd/simSums))

# 	return rankings_list
		
def calculateSimilarItems(prefs,n=10):
        # Create a dictionary of items showing which other items they
        # are most similar to.
        result={}
        # Invert the preference matrix to be item-centric
        itemPrefs=transformPrefs(prefs)
        c=0
        for item in itemPrefs:
                # Status updates for large datasets
                c+=1
                if c%100==0: print("%d / %d" % (c,len(itemPrefs)))
                # Find the most similar items to this one
                scores=topMatches(itemPrefs,item,n=n,similarity=sim_distance)
                result[item]=scores
        return result

print("Esercitazione 7 a \n")
print (user_recommendations('Toby'))
print("<-------------------------------------------------------------------->")
print("\nEsercitazione 7 b \n")
# for other in dataset:
# 	if other != 'Toby':
# 		print("Sim.Euclidea tra Toby e ",other, "=", round(similarity_score('Toby',other),2) )
print (user_recommendations3('Toby'))
		
print("<-------------------------------------------------------------------->")
print("\nEsercitazione 7 c \n")
#print (user_recommendations2('Toby'))