local longhair = require('longhair')

local str = ("abcdefghijklmopqrstuvw"):rep(7000):sub(1,100000)

for to=1,1000 do
  local v = longhair.cauchy_256_encode(100, 12, str, 1000)
  local dec = longhair.cauchy_decoder(100,12, 1000)


  local d 

  local skip = math.random(1,12)

  local decoded = false
  for i=1, #v do
    if i > skip then
      d = longhair.cauchy_decoder_push(dec, i-1, v[i])
    end
    
    if d == str then
      decoded = true
    end
  end

  print(decoded)
end
