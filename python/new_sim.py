from recommendation_data import dataset
from math import sqrt

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

def calculateAverage(person):
	sum=0
	for item in dataset[person]:
			sum+=dataset[person][item]
	return  sum / len(dataset[person])

def stima_valutazione(person,item):
    person_average = calculateAverage(person)
    other_average = 0
    sim_person = 0
    numerator_value = 0
    denominator_value = 0
    total = 0

    for other in dataset:
        if person != other:
            if item in dataset[other]:
                sim = pearson_correlation(person,other)
                if sim > 0:
                    other_average = calculateAverage(other)
                    numerator_value+= sim * (dataset[other][item]-other_average)
                    denominator_value+= sim
                    print("Indice di similarità tra",person,"and", other,">>",round(sim,2))
    total = person_average + (numerator_value/denominator_value)
    return total

def function(user):
    items = []
    value = []
    for person in dataset:
        for item in dataset[person]:
            if item not in dataset[user] and item not in items:
                    items.append(item)
    for item in items:
        value.append(round(stima_valutazione(user,item),2))
        print("Film:",item, "= ",round(stima_valutazione(user,item),2))

function('Toby')
    
   
        




