any
	---> null
	---> boolean
	---> number
				---> real
					---> long
						---> integer
	---> map<K, V>
			---> object = map<string, ?>

			---> array<V> = map<int, V>
						---> string = array<int> (= map<int, int>)
						---> interval = array<int> (= map<int, int>)

		    ---> set<V> = map<V, null>

	---> function
	---> class